# Lifecycle

### Construction
```go
// Session creation - the foundation of all clients
sess, err := session.NewSession(&aws.Config{
    Region:      aws.String("us-west-2"),
    Credentials: credentials.NewEnvCredentials(),
})
if err != nil {
    log.Fatalf("failed to create session: %v", err)
}

// Service client creation from session
svc := s3.New(sess)

// Alternative: create with additional config overrides
svc := s3.New(sess, &aws.Config{
    MaxRetries: aws.Int(5),
    LogLevel:   aws.LogLevel(aws.LogDebugWithHTTPBody),
})
```

### Destruction and Cleanup
```go
// Sessions and clients don't have explicit destructors
// They rely on Go's garbage collection

// However, response bodies must be explicitly closed
func processObject(svc *s3.S3, bucket, key string) error {
    resp, err := svc.GetObject(&s3.GetObjectInput{
        Bucket: aws.String(bucket),
        Key:    aws.String(key),
    })
    if err != nil {
        return err
    }
    defer resp.Body.Close() // Critical: close response body
    
    // Process the object
    _, err = io.Copy(os.Stdout, resp.Body)
    return err
}

// For long-running applications, close idle connections
func shutdown() {
    // Sessions don't have a Close method, but you can
    // close the underlying HTTP client's idle connections
    if transport, ok := http.DefaultTransport.(*http.Transport); ok {
        transport.CloseIdleConnections()
    }
}
```

### Move Semantics (Go doesn't have move semantics, but here's how to handle ownership)
```go
// Go doesn't have move semantics, but you can transfer ownership
// by passing pointers or using interfaces

// Transfer session ownership to a function
func createClient(sess *session.Session) *s3.S3 {
    // The caller retains ownership of sess
    return s3.New(sess)
}

// Wrap in a struct for lifecycle management
type ServiceManager struct {
    sess *session.Session
    s3   *s3.S3
    db   *dynamodb.DynamoDB
}

func NewServiceManager(sess *session.Session) *ServiceManager {
    return &ServiceManager{
        sess: sess,
        s3:   s3.New(sess),
        db:   dynamodb.New(sess),
    }
}

func (m *ServiceManager) Close() {
    // Clean up resources
    if transport, ok := m.sess.Config.HTTPClient.Transport.(*http.Transport); ok {
        transport.CloseIdleConnections()
    }
}
```

### Resource Management Patterns
```go
// Connection pooling is handled automatically
// Default HTTP client has connection pooling built-in

// Customize connection pool for high-throughput scenarios
func createOptimizedSession() *session.Session {
    transport := &http.Transport{
        MaxIdleConns:        100,
        MaxIdleConnsPerHost: 100,
        IdleConnTimeout:     90 * time.Second,
        DisableCompression:  true,
    }
    
    httpClient := &http.Client{
        Transport: transport,
        Timeout:   30 * time.Second,
    }
    
    sess := session.Must(session.NewSession(&aws.Config{
        HTTPClient: httpClient,
        Region:     aws.String("us-west-2"),
    }))
    
    return sess
}

// Use sync.Pool for frequently allocated buffers
var bufferPool = sync.Pool{
    New: func() interface{} {
        return make([]byte, 32*1024) // 32KB buffers
    },
}

func uploadLargeFile(svc *s3.S3, bucket, key string, reader io.Reader) error {
    buf := bufferPool.Get().([]byte)
    defer bufferPool.Put(buf)
    
    _, err := svc.PutObject(&s3.PutObjectInput{
        Bucket: aws.String(bucket),
        Key:    aws.String(key),
        Body:   reader,
    })
    return err
}
```