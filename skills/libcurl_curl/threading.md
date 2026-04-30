# Threading

```cpp
// Thread safety: Different handles in different threads
// SAFE: Each thread has its own handle
void worker_thread(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
}

// Launch multiple threads
std::vector<std::thread> threads;
for (const auto& url : {"http://a.com", "http://b.com", "http://c.com"}) {
    threads.emplace_back(worker_thread, url);
}
for (auto& t : threads) t.join();
```

```cpp
// Thread safety: Shared data protection
// UNSAFE: Multiple threads accessing same handle
std::string shared_data;
CURL* curl = curl_easy_init();
curl_easy_setopt(curl, CURLOPT_WRITEDATA, &shared_data);

std::thread t1([&]() { curl_easy_perform(curl); });
std::thread t2([&]() { shared_data.clear(); });  // Race condition!

// SAFE: Protect shared data with mutex
std::mutex data_mutex;
std::string safe_data;

size_t safe_callback(void* ptr, size_t size, size_t nmemb, void*) {
    std::lock_guard<std::mutex> lock(data_mutex);
    safe_data.append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}
```

```cpp
// Thread safety: Global initialization
// SAFE: Call once before creating threads
curl_global_init(CURL_GLOBAL_DEFAULT);

// Launch threads that use libcurl
std::vector<std::thread> workers;
for (int i = 0; i < 4; ++i) {
    workers.emplace_back([]() {
        CURL* curl = curl_easy_init();
        // ... use curl ...
        curl_easy_cleanup(curl);
    });
}
for (auto& t : workers) t.join();

curl_global_cleanup();  // After all threads complete
```

```cpp
// Thread safety: Share interface for shared data
// Use share interface to share cookies/DNS cache between threads
CURLSH* share = curl_share_init();
curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);

// Each thread gets its own easy handle but shares data
void thread_func() {
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_SHARE, share);
    // ... use curl ...
    curl_easy_cleanup(curl);
}

// Cleanup share after all threads
curl_share_cleanup(share);
```