# Best Practices

```cpp
// RAII wrapper for automatic cleanup
class CurlHandle {
    CURL* handle;
public:
    CurlHandle() : handle(curl_easy_init()) {
        if (!handle) throw std::runtime_error("Failed to create curl handle");
    }
    ~CurlHandle() { if (handle) curl_easy_cleanup(handle); }
    CurlHandle(const CurlHandle&) = delete;
    CurlHandle& operator=(const CurlHandle&) = delete;
    CurlHandle(CurlHandle&& other) noexcept : handle(other.handle) {
        other.handle = nullptr;
    }
    CURL* get() const { return handle; }
};
```

```cpp
// Connection reuse with connection cache
class HttpClient {
    CurlHandle handle;
public:
    HttpClient() {
        curl_easy_setopt(handle.get(), CURLOPT_FORBID_REUSE, 0L);
        curl_easy_setopt(handle.get(), CURLOPT_MAXCONNECTS, 10L);
    }
    
    std::string get(const std::string& url) {
        std::string response;
        curl_easy_setopt(handle.get(), CURLOPT_URL, url.c_str());
        curl_easy_setopt(handle.get(), CURLOPT_WRITEDATA, &response);
        
        CURLcode res = curl_easy_perform(handle.get());
        if (res != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(res));
        }
        return response;
    }
};
```

```cpp
// Proper timeout and retry configuration
curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);           // Total timeout
curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);    // Connection timeout
curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 30L);    // Low speed timeout
curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1024L); // Bytes/second

// Retry logic
int retries = 3;
CURLcode res;
do {
    res = curl_easy_perform(curl);
    if (res == CURLE_OPERATION_TIMEDOUT || res == CURLE_COULDNT_CONNECT) {
        sleep(1);  // Wait before retry
    }
} while (res != CURLE_OK && retries-- > 0);
```

```cpp
// Error handling with detailed diagnostics
struct ErrorBuffer {
    char error[CURL_ERROR_SIZE];
};

ErrorBuffer err;
curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, &err);

CURLcode res = curl_easy_perform(curl);
if (res != CURLE_OK) {
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    
    char* effective_url = nullptr;
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effective_url);
    
    std::cerr << "Request to " << (effective_url ? effective_url : "unknown")
              << " failed: " << curl_easy_strerror(res)
              << " (HTTP " << http_code << ")"
              << " Details: " << err.error << std::endl;
}
```