# Threading

### Thread Safety Guarantees
```go
// Service clients are safe for concurrent use after creation
var svc *s3.S3

func init() {
    sess := session.Must(session.NewSession())
    svc = s3.New(sess)
}

// Multiple goroutines can safely use the same client
func concurrentAccess() {
    var wg sync.WaitGroup
    
    for i := 0; i < 10; i++ {
        wg.Add(1)
        go func(id int) {
            defer wg.Done()
            
            // Safe: concurrent calls to same client
            _, err := svc.PutObject(&s3.PutObjectInput{
                Bucket: aws.String("my-bucket"),
                Key:    aws.String(fmt.Sprintf("key-%d", id)),
                Body:   bytes.NewReader([]byte(fmt.Sprintf("data-%d", id))),
            })
            if err != nil {
                log.Printf("goroutine %d failed: %v", id, err)
            }
        }(i)
    }
    
    wg.Wait()
}
```

### Session Thread Safety
```go
// Sessions are safe for concurrent use
var globalSess *session.Session

func init() {
    globalSess = session.Must(session.NewSession())
}

// Multiple goroutines can create clients from the same session
func createClientsConcurrently() {
    var wg sync.WaitGroup
    
    for i := 0; i < 5; i++ {
        wg.Add(1)
        go func() {
            defer wg.Done()
            
            // Safe: creating clients from shared session
            s3Client := s3.New(globalSess)
            dynamoClient := dynamodb.New(globalSess)
            
            _ = s3Client
            _ = dynamoClient
        }()
    }
    
    wg.Wait()
}
```

### Concurrent Request Patterns
```go
// BAD: Sequential requests in goroutines (no parallelism)
func processItems(items []string) {
    for _, item := range items {
        go func(i string) {
            // Each goroutine still makes sequential calls
            resp1, _ := svc.GetObject(&s3.GetObjectInput{Key: aws.String(i + "-1")})
            resp1.Body.Close()
            resp2, _ := svc.GetObject(&s3.GetObjectInput{Key: aws.String(i + "-2")})
            resp2.Body.Close()
        }(item)
    }
}

// GOOD: Parallel requests with proper concurrency control
func processItemsParallel(items []string, maxConcurrent int) {
    sem := make(chan struct{}, maxConcurrent)
    var wg sync.WaitGroup
    
    for _, item := range items {
        wg.Add(1)
        sem <- struct{}{}
        
        go func(i string) {
            defer wg.Done()
            defer func() { <-sem }()
            
            // Make parallel requests
            var innerWg sync.WaitGroup
            innerWg.Add(2)
            
            go func() {
                defer innerWg.Done()
                resp, err := svc.GetObject(&s3.GetObjectInput{Key: aws.String(i + "-1")})
                if err == nil {
                    resp.Body.Close()
                }
            }()
            
            go func() {
                defer innerWg.Done()
                resp, err := svc.GetObject(&s3.GetObjectInput{Key: aws.String(i + "-2")})
                if err == nil {
                    resp.Body.Close()
                }
            }()
            
            innerWg.Wait()
        }(item)
    }
    
    wg.Wait()
}
```

### Shared State Protection
```go
// BAD: Shared state without synchronization
var counter int

func incrementCounter() {
    svc.GetObject(&s3.GetObjectInput{
        Bucket: aws.String("my-bucket"),
        Key:    aws.String("my-key"),
    })
    counter++ // Race condition
}

// GOOD: Protect shared state with mutex
type SafeCounter struct {
    mu      sync.Mutex
    counter int
    svc     *s3.S3
}

func (c *SafeCounter) Increment() {
    c.mu.Lock()
    defer c.mu.Unlock()
    
    c.svc.GetObject(&s3.GetObjectInput{
        Bucket: aws.String("my-bucket"),
        Key:    aws.String("my-key"),
    })
    c.counter++
}
```

### Context Propagation in Concurrent Operations
```go
// BAD: Context not propagated to concurrent operations
func processWithTimeout(ctx context.Context, keys []string) {
    for _, key := range keys {
        go func(k string) {
            // ctx is not used, timeout won't work
            svc.GetObject(&s3.GetObjectInput{Key: aws.String(k)})
        }(key)
    }
}

// GOOD: Propagate context to all concurrent operations
func processWithTimeout(ctx context.Context, keys []string) error {
    errCh := make(chan error, len(keys))
    
    for _, key := range keys {
        go func(k string) {
            select {
            case <-ctx.Done():
                errCh <- ctx.Err()
                return
            default:
                _, err := svc.GetObjectWithContext(ctx, &s3.GetObjectInput{
                    Bucket: aws.String("my-bucket"),
                    Key:    aws.String(k),
                })
                errCh <- err
            }
        }(key)
    }
    
    // Collect results
    for i := 0; i < len(keys); i++ {
        if err := <-errCh; err != nil {
            return err
        }
    }
    
    return nil
}
```