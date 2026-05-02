# Safety

### Condition 1: NEVER use session.Must without understanding it panics
```go
// BAD: Panics on invalid config, no recovery possible
sess := session.Must(session.NewSession(&aws.Config{
    Region: aws.String("invalid-region"),
}))

// GOOD: Handle session creation errors
sess, err := session.NewSession(&aws.Config{
    Region: aws.String("invalid-region"),
})
if err != nil {
    log.Printf("session creation failed: %v, using default", err)
    sess, _ = session.NewSession() // fallback to default
}
```

### Condition 2: NEVER ignore context cancellation in long-running operations
```go
// BAD: Upload hangs indefinitely if network is slow
svc.PutObject(&s3.PutObjectInput{
    Bucket: aws.String("my-bucket"),
    Key:    aws.String("large-file"),
    Body:   largeFile,
})

// GOOD: Always set timeout for network operations
ctx, cancel := context.WithTimeout(context.Background(), 5*time.Minute)
defer cancel()
_, err := svc.PutObjectWithContext(ctx, &s3.PutObjectInput{
    Bucket: aws.String("my-bucket"),
    Key:    aws.String("large-file"),
    Body:   largeFile,
})
```

### Condition 3: NEVER share credentials in plaintext or hardcode them
```go
// BAD: Hardcoded credentials in source code
sess := session.Must(session.NewSession(&aws.Config{
    Credentials: credentials.NewStaticCredentials("AKID", "SECRET", ""),
}))

// GOOD: Use environment variables or IAM roles
os.Setenv("AWS_ACCESS_KEY_ID", "AKID")
os.Setenv("AWS_SECRET_ACCESS_KEY", "SECRET")
sess := session.Must(session.NewSession())
```

### Condition 4: NEVER forget to close response bodies
```go
// BAD: Response body leak causes connection pool exhaustion
for i := 0; i < 1000; i++ {
    resp, _ := svc.GetObject(&s3.GetObjectInput{
        Bucket: aws.String("my-bucket"),
        Key:    aws.String(fmt.Sprintf("key-%d", i)),
    })
    // resp.Body is never closed
}

// GOOD: Always defer close
for i := 0; i < 1000; i++ {
    resp, err := svc.GetObject(&s3.GetObjectInput{
        Bucket: aws.String("my-bucket"),
        Key:    aws.String(fmt.Sprintf("key-%d", i)),
    })
    if err != nil {
        continue
    }
    resp.Body.Close() // Close immediately or defer
}
```

### Condition 5: NEVER assume pagination returns all results
```go
// BAD: Only processes first page
result, _ := svc.Scan(&dynamodb.ScanInput{
    TableName: aws.String("LargeTable"),
})
for _, item := range result.Items {
    processItem(item)
}

// GOOD: Handle pagination correctly
input := &dynamodb.ScanInput{
    TableName: aws.String("LargeTable"),
}
for {
    result, err := svc.Scan(input)
    if err != nil {
        log.Fatal(err)
    }
    for _, item := range result.Items {
        processItem(item)
    }
    if result.LastEvaluatedKey == nil {
        break
    }
    input.ExclusiveStartKey = result.LastEvaluatedKey
}
```