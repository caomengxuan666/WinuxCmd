# Best Practices

### Session Management
```go
// Create session once and reuse
var globalSession *session.Session

func init() {
    sess, err := session.NewSession(&aws.Config{
        Region: aws.String("us-west-2"),
        MaxRetries: aws.Int(3),
    })
    if err != nil {
        log.Fatalf("failed to create session: %v", err)
    }
    globalSession = sess
}

// Create service clients from shared session
func getS3Client() *s3.S3 {
    return s3.New(globalSession)
}
```

### Error Handling with Structured Logging
```go
func handleS3Operation(bucket, key string) error {
    svc := s3.New(globalSession)
    
    resp, err := svc.GetObject(&s3.GetObjectInput{
        Bucket: aws.String(bucket),
        Key:    aws.String(key),
    })
    if err != nil {
        if aerr, ok := err.(awserr.Error); ok {
            switch aerr.Code() {
            case "NoSuchKey":
                return fmt.Errorf("object %s/%s not found", bucket, key)
            case "AccessDenied":
                return fmt.Errorf("access denied to %s/%s", bucket, key)
            default:
                return fmt.Errorf("AWS error %s: %s", aerr.Code(), aerr.Message())
            }
        }
        return fmt.Errorf("unexpected error: %v", err)
    }
    defer resp.Body.Close()
    
    // Process response
    return nil
}
```

### Context Propagation
```go
func uploadWithRetry(ctx context.Context, svc *s3.S3, input *s3.PutObjectInput) error {
    var lastErr error
    for i := 0; i < 3; i++ {
        select {
        case <-ctx.Done():
            return ctx.Err()
        default:
            _, err := svc.PutObjectWithContext(ctx, input)
            if err == nil {
                return nil
            }
            lastErr = err
            time.Sleep(time.Duration(i+1) * time.Second)
        }
    }
    return fmt.Errorf("upload failed after 3 retries: %v", lastErr)
}
```

### Configuration Management
```go
type AWSConfig struct {
    Region          string
    Endpoint        string
    MaxRetries      int
    Timeout         time.Duration
}

func NewSessionFromConfig(cfg AWSConfig) (*session.Session, error) {
    awsCfg := &aws.Config{
        Region:     aws.String(cfg.Region),
        MaxRetries: aws.Int(cfg.MaxRetries),
    }
    
    if cfg.Endpoint != "" {
        awsCfg.Endpoint = aws.String(cfg.Endpoint)
    }
    
    if cfg.Timeout > 0 {
        awsCfg.HTTPClient = &http.Client{
            Timeout: cfg.Timeout,
        }
    }
    
    return session.NewSession(awsCfg)
}
```

### Resource Cleanup
```go
type S3Manager struct {
    svc  *s3.S3
    pool *sync.Pool
}

func NewS3Manager(sess *session.Session) *S3Manager {
    return &S3Manager{
        svc: s3.New(sess),
        pool: &sync.Pool{
            New: func() interface{} {
                return &bytes.Buffer{}
            },
        },
    }
}

func (m *S3Manager) GetObject(ctx context.Context, bucket, key string) ([]byte, error) {
    resp, err := m.svc.GetObjectWithContext(ctx, &s3.GetObjectInput{
        Bucket: aws.String(bucket),
        Key:    aws.String(key),
    })
    if err != nil {
        return nil, err
    }
    defer resp.Body.Close()
    
    buf := m.pool.Get().(*bytes.Buffer)
    defer m.pool.Put(buf)
    buf.Reset()
    
    _, err = io.Copy(buf, resp.Body)
    if err != nil {
        return nil, err
    }
    
    data := make([]byte, buf.Len())
    copy(data, buf.Bytes())
    return data, nil
}
```