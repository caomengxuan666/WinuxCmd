# Overview

```cpp
// Overview: rustls - Modern TLS in Rust, usable from C++
// 
// rustls is a TLS library that provides cryptographic security without
// requiring configuration. It implements TLS 1.2 and TLS 1.3 for both
// clients and servers.

// When to use rustls:
// - Need TLS with modern security standards
// - Want to avoid OpenSSL complexity
// - Require platform-independent TLS
// - Need post-quantum cryptography support (with aws-lc-rs provider)

// When NOT to use rustls:
// - Need FIPS 140-2/140-3 compliance (use OpenSSL)
// - Require legacy TLS versions (TLS 1.0/1.1)
// - Need DTLS support
// - Must use hardware security modules (HSMs)

// Key design principles:
// 1. Security by default - no unsafe features or obsolete cryptography
// 2. Provider-based cryptography - choose crypto backend
// 3. No configuration needed for basic security

// Provider selection (required since 0.24):
#include <rustls.h>

void provider_selection() {
    // Option 1: aws-lc-rs (recommended for performance + post-quantum)
    // Cargo.toml: rustls-aws-lc-rs = "0.24"
    
    // Option 2: ring (easier cross-platform build)
    // Cargo.toml: rustls-ring = "0.24"
    
    // Provider must be explicitly specified
    rustls_crypto_provider* provider = rustls_crypto_provider_aws_lc_rs_new();
    rustls_client_config_builder* builder = 
        rustls_client_config_builder_new_with_provider(provider);
}
```