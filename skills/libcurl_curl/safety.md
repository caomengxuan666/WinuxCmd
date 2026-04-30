# Safety

```cpp
// RED LINE 1: NEVER use curl_easy_cleanup on a NULL handle
CURL* curl = nullptr;
curl_easy_cleanup(curl);  // CRASH: undefined behavior

// GOOD: Always check before cleanup
if (curl) {
    curl_easy_cleanup(curl);
}
```

```cpp
// RED LINE 2: NEVER modify callback data while transfer is in progress
std::string data;
curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
// BAD: Modifying data while curl_easy_perform is running
std::thread t([&]() { data.clear(); });  // Race condition!
curl_easy_perform(curl);

// GOOD: Only access data after transfer completes
curl_easy_perform(curl);
// Now safe to use data
```

```cpp
// RED LINE 3: NEVER call curl functions from signal handlers
void signal_handler(int sig) {
    curl_easy_cleanup(curl);  // CRASH: not signal-safe
}

// GOOD: Use self-pipe trick or eventfd for signal handling
```

```cpp
// RED LINE 4: NEVER use the same easy handle in multiple threads simultaneously
// BAD: Concurrent access to same handle
std::thread t1([&]() { curl_easy_perform(curl); });
std::thread t2([&]() { curl_easy_setopt(curl, CURLOPT_URL, "new"); });

// GOOD: Each thread gets its own handle
std::thread t1([&]() { 
    CURL* c = curl_easy_init();
    curl_easy_perform(c);
    curl_easy_cleanup(c);
});
```

```cpp
// RED LINE 5: NEVER ignore CURLE_WRITE_ERROR from callbacks
size_t bad_callback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    return 0;  // Signals error but curl may not handle gracefully
}

// GOOD: Return actual bytes written or signal error properly
size_t good_callback(void* ptr, size_t size, size_t nmemb, std::string* data) {
    if (data->capacity() - data->size() < size * nmemb) {
        return 0;  // Signal out of memory
    }
    data->append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}
```