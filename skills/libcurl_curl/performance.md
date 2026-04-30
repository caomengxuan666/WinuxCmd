# Performance

```cpp
// Connection reuse for better performance
// BAD: Creating new connection for each request
for (const auto& url : urls) {
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
}

// GOOD: Reuse connection with same handle
CURL* curl = curl_easy_init();
curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 0L);  // Allow connection reuse
for (const auto& url : urls) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_perform(curl);
}
curl_easy_cleanup(curl);
```

```cpp
// Buffer size optimization
// Default buffer size is usually fine, but can be tuned
curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 102400L);  // 100KB buffer

// For large downloads, use streaming instead of buffering
size_t stream_callback(void* ptr, size_t size, size_t nmemb, FILE* file) {
    return fwrite(ptr, size, nmemb, file);  // Direct to disk
}
```

```cpp
// Multi-handle for concurrent transfers
// BAD: Sequential transfers
for (const auto& url : urls) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_perform(curl);  // Blocks until complete
}

// GOOD: Parallel transfers with multi interface
CURLM* multi = curl_multi_init();
for (const auto& url : urls) {
    CURL* easy = curl_easy_init();
    curl_easy_setopt(easy, CURLOPT_URL, url.c_str());
    curl_multi_add_handle(multi, easy);
}

int still_running;
do {
    curl_multi_perform(multi, &still_running);
    curl_multi_poll(multi, nullptr, 0, 100, nullptr);
} while (still_running);
```

```cpp
// Memory allocation patterns
// Pre-allocate response buffer to avoid reallocations
std::string response;
response.reserve(1024 * 1024);  // Reserve 1MB

// Use custom allocators if needed
curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* ptr, size_t size, 
    size_t nmemb, std::string* data) {
    size_t total = size * nmemb;
    data->append(static_cast<char*>(ptr), total);
    return total;
});
```