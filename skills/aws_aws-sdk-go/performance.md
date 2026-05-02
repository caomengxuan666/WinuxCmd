# Performance

### Connection Pooling
```go
// Default connection pool may be insufficient for high throughput
// BAD: Using default HTTP client
sess := session.Must(session.NewSession())

// GOOD: Customize HTTP transport for better performance
transport := &http.Transport{
    MaxIdleConns:        200,
    MaxIdleConnsPerHost: 50,
    IdleConnTimeout:     90 * time.Second,
    DisableCompression:  true, // AWS already compresses responses
}

httpClient := &http.Client{
    Transport: transport,
    Timeout:   30 * time.Second,
}

sess := session.Must(session.NewSession(&aws.Config{
    HTTPClient: httpClient,
}))
```

### Memory Allocation Patterns
```go
// BAD: Frequent allocations in hot path
func processObjects(svc *s3.S3, keys []string) {
    for _, key := range keys {
        resp, _ := svc.GetObject(&s3.GetObjectInput{
            Bucket: aws.String("my-bucket"),
            Key:    aws.String(key),
        })
        // Each call allocates new response struct
        data, _ := ioutil.ReadAll(resp.Body)
        resp.Body.Close()
        process(data)
    }
}

// GOOD: Reuse buffers and minimize allocations
var bufPool = sync.Pool{
    New: func() interface{} {
        return &bytes.Buffer{}
    },
}

func processObjectsOptimized(svc *s3.S3, keys []string) {
    for _, key := range keys {
        resp, err := svc.GetObject(&s3.GetObjectInput{
            Bucket: aws.String("my-bucket"),
            Key:    aws.String(key),
        })
        if err != nil {
            continue
        }
        
        buf := bufPool.Get().(*bytes.Buffer)
        buf.Reset()
        io.Copy(buf, resp.Body)
        resp.Body.Close()
        
        process(buf.Bytes())
        bufPool.Put(buf)
    }
}
```

### Retry and Backoff Optimization
```go
// BAD: Default retryer may cause unnecessary delays
sess := session.Must(session.NewSession())

// GOOD: Custom retryer for specific use cases
type fastRetryer struct {
    maxRetries int
}

func (r *fastRetryer) MaxRetries() int {
    return r.maxRetries
}

func (r *fastRetryer) RetryRules(req *request.Request) time.Duration {
    // Faster backoff for time-sensitive operations
    return time.Duration(req.RetryCount) * 100 * time.Millisecond
}

func (r *fastRetryer) ShouldRetry(req *request.Request) bool {
    // Only retry on specific errors
    if req.Error != nil {
        if aerr, ok := req.Error.(awserr.Error); ok {
            switch aerr.Code() {
            case "RequestError", "Throttling", "ProvisionedThroughputExceededException":
                return req.RetryCount < r.maxRetries
            }
        }
    }
    return false
}

sess := session.Must(session.NewSession(&aws.Config{
    Retryer: &fastRetryer{maxRetries: 3},
}))
```

### Batch Operations
```go
// BAD: Sequential individual operations
func uploadFiles(svc *s3.S3, files []string) {
    for _, file := range files {
        svc.PutObject(&s3.PutObjectInput{
            Bucket: aws.String("my-bucket"),
            Key:    aws.String(file),
            Body:   openFile(file),
        })
    }
}

// GOOD: Concurrent batch operations with worker pool
func uploadFilesConcurrent(svc *s3.S3, files []string, concurrency int) {
    sem := make(chan struct{}, concurrency)
    var wg sync.WaitGroup
    
    for _, file := range files {
        wg.Add(1)
        sem <- struct{}{}
        
        go func(f string) {
            defer wg.Done()
            defer func() { <-sem }()
            
            _, err := svc.PutObject(&s3.PutObjectInput{
                Bucket: aws.String("my-bucket"),
                Key:    aws.String(f),
                Body:   openFile(f),
            })
            if err != nil {
                log.Printf("failed to upload %s: %v", f, err)
            }
        }(file)
    }
    
    wg.Wait()
}
```

### Response Size Optimization
```go
// BAD: Requesting all fields when only few are needed
result, _ := svc.DescribeInstances(&ec2.DescribeInstancesInput{})

// GOOD: Use filters and select specific fields
result, _ := svc.DescribeInstances(&ec2.DescribeInstancesInput{
    Filters: []*ec2.Filter{
        {
            Name:   aws.String("instance-state-name"),
            Values: []*string{aws.String("running")},
        },
    },
    MaxResults: aws.Int64(100),
})
```