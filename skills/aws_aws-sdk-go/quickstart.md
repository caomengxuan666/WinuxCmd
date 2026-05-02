# Quickstart

```go
package main

import (
	"context"
	"fmt"
	"os"
	"time"

	"github.com/aws/aws-sdk-go/aws"
	"github.com/aws/aws-sdk-go/aws/session"
	"github.com/aws/aws-sdk-go/service/s3"
	"github.com/aws/aws-sdk-go/service/dynamodb"
	"github.com/aws/aws-sdk-go/service/sqs"
)

// Pattern 1: Basic S3 upload
func basicS3Upload() {
	sess := session.Must(session.NewSession())
	svc := s3.New(sess)

	_, err := svc.PutObject(&s3.PutObjectInput{
		Bucket: aws.String("my-bucket"),
		Key:    aws.String("my-key"),
		Body:   os.Stdin,
	})
	if err != nil {
		fmt.Printf("upload failed: %v\n", err)
	}
}

// Pattern 2: S3 upload with context timeout
func s3UploadWithTimeout() {
	sess := session.Must(session.NewSession())
	svc := s3.New(sess)

	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Minute)
	defer cancel()

	_, err := svc.PutObjectWithContext(ctx, &s3.PutObjectInput{
		Bucket: aws.String("my-bucket"),
		Key:    aws.String("my-key"),
		Body:   os.Stdin,
	})
	if err != nil {
		fmt.Printf("upload failed: %v\n", err)
	}
}

// Pattern 3: DynamoDB query with pagination
func dynamoDBQuery() {
	sess := session.Must(session.NewSession())
	svc := dynamodb.New(sess)

	input := &dynamodb.QueryInput{
		TableName: aws.String("Users"),
		KeyConditionExpression: aws.String("PK = :pk"),
		ExpressionAttributeValues: map[string]*dynamodb.AttributeValue{
			":pk": {S: aws.String("user#123")},
		},
	}

	result, err := svc.Query(input)
	if err != nil {
		fmt.Printf("query failed: %v\n", err)
	}
	fmt.Printf("found %d items\n", len(result.Items))
}

// Pattern 4: SQS message sending
func sendSQSMessage() {
	sess := session.Must(session.NewSession())
	svc := sqs.New(sess)

	_, err := svc.SendMessage(&sqs.SendMessageInput{
		QueueUrl:    aws.String("https://sqs.us-east-1.amazonaws.com/123456789012/MyQueue"),
		MessageBody: aws.String("Hello, SQS!"),
	})
	if err != nil {
		fmt.Printf("send failed: %v\n", err)
	}
}

// Pattern 5: S3 multipart upload
func multipartUpload() {
	sess := session.Must(session.NewSession())
	svc := s3.New(sess)

	uploader := s3.NewUploader(sess)
	_, err := uploader.Upload(&s3.UploadInput{
		Bucket: aws.String("my-bucket"),
		Key:    aws.String("large-file.zip"),
		Body:   os.Stdin,
	})
	if err != nil {
		fmt.Printf("multipart upload failed: %v\n", err)
	}
}

// Pattern 6: Error handling with awserr
func handleAWSError() {
	sess := session.Must(session.NewSession())
	svc := s3.New(sess)

	_, err := svc.GetObject(&s3.GetObjectInput{
		Bucket: aws.String("my-bucket"),
		Key:    aws.String("nonexistent-key"),
	})
	if err != nil {
		if aerr, ok := err.(awserr.Error); ok {
			switch aerr.Code() {
			case s3.ErrCodeNoSuchKey:
				fmt.Println("object not found")
			default:
				fmt.Printf("AWS error: %s - %s\n", aerr.Code(), aerr.Message())
			}
		} else {
			fmt.Printf("non-AWS error: %v\n", err)
		}
	}
}

// Pattern 7: Custom retry configuration
func customRetryConfig() {
	sess := session.Must(session.NewSession(&aws.Config{
		MaxRetries: aws.Int(5),
		Retryer:    &customRetryer{},
	}))
	svc := s3.New(sess)
	_ = svc
}

type customRetryer struct{}

func (r *customRetryer) MaxRetries() int { return 5 }
func (r *customRetryer) RetryRules(req *request.Request) time.Duration {
	return time.Duration(req.RetryCount) * time.Second
}
func (r *customRetryer) ShouldRetry(req *request.Request) bool {
	return req.RetryCount < 5
}

// Pattern 8: Cross-region S3 access
func crossRegionS3() {
	sess := session.Must(session.NewSession(&aws.Config{
		Region: aws.String("us-west-2"),
	}))
	svc := s3.New(sess)

	_, err := svc.GetBucketLocation(&s3.GetBucketLocationInput{
		Bucket: aws.String("my-bucket"),
	})
	if err != nil {
		fmt.Printf("failed to get bucket location: %v\n", err)
	}
}
```