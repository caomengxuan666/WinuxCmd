# Pitfalls

```cpp
// BAD: Forgetting to initialize global state
CURL* curl = curl_easy_init();  // May fail if global not initialized
curl_easy_setopt(curl, CURLOPT_URL, "http://example.com");
curl_easy_perform(curl);
curl_easy_cleanup(curl);
```

```cpp
// GOOD: Always initialize global state first
curl_global_init(CURL_GLOBAL_DEFAULT);
CURL* curl = curl_easy_init();
if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://example.com");
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
}
curl_global_cleanup();
```

```cpp
// BAD: Not checking return values
CURL* curl = curl_easy_init();
curl_easy_setopt(curl, CURLOPT_URL, "http://example.com");
curl_easy_perform(curl);  // Ignoring potential failure
curl_easy_cleanup(curl);
```

```cpp
// GOOD: Always check return codes
CURL* curl = curl_easy_init();
if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://example.com");
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    curl_easy_cleanup(curl);
}
```

```cpp
// BAD: Memory leak from callback data
size_t bad_callback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    char* buffer = new char[size * nmemb];  // Never freed!
    memcpy(buffer, ptr, size * nmemb);
    return size * nmemb;
}
```

```cpp
// GOOD: Proper memory management in callbacks
size_t good_callback(void* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}
```

```cpp
// BAD: Disabling SSL verification in production
curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  // Security risk!
curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);  // Security risk!
```

```cpp
// GOOD: Proper SSL verification
curl_easy_setopt(curl, CURLOPT_CAINFO, "/etc/ssl/certs/ca-certificates.crt");
curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
```