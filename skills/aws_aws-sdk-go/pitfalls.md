# Pitfalls

### Pitfall 1: Not checking for nil when using session.Must
```go
// BAD: session.Must panics if session creation fails
sess := session.Must(session.NewSession())
// If AWS config is invalid, this panics without recovery

// GOOD: Handle session creation errors gracefully
sess, err := session.NewSession()
if err != nil {
    log.Fatalf("failed to create session: %v", err)
}
```

### Pitfall 2: Forgetting to close response bodies
```go
// BAD: Response body is never closed, causing resource leak
resp, _ := svc.GetObject(&s3.GetObjectInput{
    Bucket: aws.String("my-bucket"),
    Key:    aws.String("my-key"),
})
data, _ := ioutil.ReadAll(resp.Body)

// GOOD: Always close response bodies
resp, err := svc.GetObject(&s3.GetObjectInput{
    Bucket: aws.String("my-bucket"),
    Key:    aws.String("my-key"),
})
if err != nil {
    log.Fatal(err)
}
defer resp.Body.Close()
data, _ := ioutil.ReadAll(resp.Body)
```

### Pitfall 3: Ignoring context cancellation
```go
// BAD: No timeout or cancellation support
svc.PutObject(&s3.PutObjectInput{
    Bucket: aws.String("my-bucket"),
    Key:    aws.String("large-file"),
    Body:   largeFile,
})

// GOOD: Use context for timeout and cancellation
ctx, cancel := context.WithTimeout(context.Background(), 30*time.Second)
defer cancel()
_, err := svc.PutObjectWithContext(ctx, &s3.PutObjectInput{
    Bucket: aws.String("my-bucket"),
    Key:    aws.String("large-file"),
    Body:   largeFile,
})
```

### Pitfall 4: Assuming zero values are sent as parameters
```go
// BAD: Zero values are not sent to AWS API
input := &dynamodb.PutItemInput{
    TableName: aws.String("Users"),
    Item: map[string]*dynamodb.AttributeValue{
        "age": {N: aws.String("0")}, // This sends "0" as a string
    },
}

// GOOD: Use pointer types correctly
input := &dynamodb.PutItemInput{
    TableName: aws.String("Users"),
    Item: map[string]*dynamodb.AttributeValue{
        "age": {N: aws.String("25")},
    },
}
```

### Pitfall 5: Not handling pagination
```go
// BAD: Only gets first page of results
result, _ := svc.ListObjectsV2(&s3.ListObjectsV2Input{
    Bucket: aws.String("my-bucket"),
})

// GOOD: Handle pagination properly
input := &s3.ListObjectsV2Input{
    Bucket: aws.String("my-bucket"),
}
for {
    result, err := svc.ListObjectsV2(input)
    if err != nil {
        log.Fatal(err)
    }
    for _, obj := range result.Contents {
        fmt.Println(*obj.Key)
    }
    if result.IsTruncated == nil || !*result.IsTruncated {
        break
    }
    input.ContinuationToken = result.NextContinuationToken
}
```

### Pitfall 6: Creating new sessions for every request
```go
// BAD: Creates new session for each request (expensive)
func uploadFile(key string) {
    sess := session.Must(session.NewSession())
    svc := s3.New(sess)
    svc.PutObject(&s3.PutObjectInput{...})
}

// GOOD: Reuse session across requests
var sess = session.Must(session.NewSession())

func uploadFile(key string) {
    svc := s3.New(sess)
    svc.PutObject(&s3.PutObjectInput{...})
}
```

### Pitfall 7: Not checking for specific error codes
```go
// BAD: Generic error handling misses important details
err := svc.PutObject(&s3.PutObjectInput{...})
if err != nil {
    log.Printf("upload failed: %v", err)
}

// GOOD: Check for specific AWS error codes
err := svc.PutObject(&s3.PutObjectInput{...})
if err != nil {
    if aerr, ok := err.(awserr.Error); ok {
        switch aerr.Code() {
        case "NoSuchBucket":
            log.Fatal("bucket does not exist")
        case "AccessDenied":
            log.Fatal("permission denied")
        default:
            log.Printf("AWS error: %s - %s", aerr.Code(), aerr.Message())
        }
    } else {
        log.Printf("non-AWS error: %v", err)
    }
}
```

### Pitfall 8: Using default retryer for idempotent operations
```go
// BAD: Default retryer may retry non-idempotent operations
svc.PutObject(&s3.PutObjectInput{
    Bucket: aws.String("my-bucket"),
    Key:    aws.String("my-key"),
    Body:   bytes.NewReader(data),
})

// GOOD: Configure retry behavior appropriately
sess := session.Must(session.NewSession(&aws.Config{
    MaxRetries: aws.Int(3),
    Retryer: &customRetryer{
        maxRetries: 3,
        shouldRetry: func(req *request.Request) bool {
            return req.HTTPRequest.Method == "GET" || req.HTTPRequest.Method == "HEAD"
        },
    },
}))
```