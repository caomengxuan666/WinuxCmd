# Overview

The AWS SDK for Go (v1) is the official Go library for interacting with Amazon Web Services. It provides client packages for all AWS services (S3, DynamoDB, SQS, Lambda, etc.) along with shared infrastructure for configuration, authentication, request signing, retry logic, and error handling.

**When to use:**
- Building Go applications that need to call AWS APIs
- Migrating existing Go v1 SDK code (though v2 is now recommended)
- Working with legacy systems that depend on v1 SDK behavior
- Projects where v2 migration is not immediately feasible

**When NOT to use:**
- New projects should use [AWS SDK for Go v2](https://aws.github.io/aws-sdk-go-v2/docs/) as v1 is end-of-support as of July 31, 2025
- Applications requiring the latest AWS service features (v2 has better coverage)
- Projects needing improved performance and reduced memory allocations (v2 is more efficient)

**Key design principles:**
- **Session-based configuration**: All clients are created from a shared `session.Session` that holds credentials, region, and other settings
- **Pointer types for optional fields**: AWS API parameters use pointer types (via `aws.String()`, `aws.Int64()`) to distinguish between zero values and unset fields
- **Context-aware operations**: Most service operations have `WithContext` variants for cancellation and timeout
- **Automatic retry**: Built-in retry logic with exponential backoff for transient failures
- **Concurrent-safe clients**: Service clients are safe for concurrent use once created