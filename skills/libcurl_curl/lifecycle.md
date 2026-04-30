# Lifecycle

```cpp
// Construction and initialization
// Global initialization must be called once per process
curl_global_init(CURL_GLOBAL_DEFAULT);  // Called once at startup

// Create easy handle
CURL* curl = curl_easy_init();  // Returns NULL on failure
if (!curl) {
    // Handle allocation failure
}

// Configure the handle
curl_easy_setopt(curl, CURLOPT_URL, "http://example.com");
```

```cpp
// Destruction and cleanup
// Cleanup individual handle
curl_easy_cleanup(curl);  // Frees all resources associated with handle
curl = nullptr;  // Prevent dangling pointer

// Global cleanup at shutdown
curl_global_cleanup();  // Must be called after all handles are cleaned up
```

```cpp
// Resource management with reset
// Reuse handle for different requests
curl_easy_reset(curl);  // Reset all options to defaults
curl_easy_setopt(curl, CURLOPT_URL, "http://new-url.com");
// Handle is ready for new transfer

// Or create fresh handle
curl_easy_cleanup(curl);
curl = curl_easy_init();
```

```cpp
// Move semantics (manual implementation since CURL* is opaque)
class CurlResource {
    CURL* handle;
public:
    CurlResource() : handle(curl_easy_init()) {}
    
    // Move constructor
    CurlResource(CurlResource&& other) noexcept 
        : handle(other.handle) {
        other.handle = nullptr;
    }
    
    // Move assignment
    CurlResource& operator=(CurlResource&& other) noexcept {
        if (this != &other) {
            if (handle) curl_easy_cleanup(handle);
            handle = other.handle;
            other.handle = nullptr;
        }
        return *this;
    }
    
    ~CurlResource() {
        if (handle) curl_easy_cleanup(handle);
    }
    
    // No copy semantics
    CurlResource(const CurlResource&) = delete;
    CurlResource& operator=(const CurlResource&) = delete;
};
```