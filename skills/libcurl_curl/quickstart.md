# Quickstart

```cpp
// Basic HTTP GET request
#include <curl/curl.h>
#include <string>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if (curl) {
        std::string response;
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.example.com/data");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        
        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            // Use response string
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}
```

```cpp
// POST request with JSON body
#include <curl/curl.h>
#include <string>
#include <nlohmann/json.hpp>

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if (curl) {
        std::string response;
        nlohmann::json jsonData = {{"key", "value"}, {"name", "test"}};
        std::string postData = jsonData.dump();
        
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.example.com/submit");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postData.length());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, 
            curl_slist_append(nullptr, "Content-Type: application/json"));
        
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}
```

```cpp
// Download file to disk
#include <curl/curl.h>
#include <fstream>

size_t WriteFileCallback(void* ptr, size_t size, size_t nmemb, std::ofstream* stream) {
    stream->write(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if (curl) {
        std::ofstream file("downloaded_file.pdf", std::ios::binary);
        curl_easy_setopt(curl, CURLOPT_URL, "https://example.com/file.pdf");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFileCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
        
        CURLcode res = curl_easy_perform(curl);
        file.close();
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}
```

```cpp
// HTTPS with certificate verification
#include <curl/curl.h>

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://secure.example.com");
        curl_easy_setopt(curl, CURLOPT_CAINFO, "/etc/ssl/certs/ca-certificates.crt");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
        
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}
```

```cpp
// Multi-handle for concurrent requests
#include <curl/curl.h>
#include <vector>

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURLM* multi_handle = curl_multi_init();
    
    std::vector<CURL*> handles;
    for (const auto& url : {"http://example.com", "http://example.org"}) {
        CURL* easy = curl_easy_init();
        curl_easy_setopt(easy, CURLOPT_URL, url);
        curl_multi_add_handle(multi_handle, easy);
        handles.push_back(easy);
    }
    
    int still_running = 0;
    do {
        curl_multi_perform(multi_handle, &still_running);
        curl_multi_poll(multi_handle, nullptr, 0, 1000, nullptr);
    } while (still_running);
    
    for (auto* easy : handles) {
        curl_multi_remove_handle(multi_handle, easy);
        curl_easy_cleanup(easy);
    }
    curl_multi_cleanup(multi_handle);
    curl_global_cleanup();
    return 0;
}
```