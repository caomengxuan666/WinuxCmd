# Best Practices

```cpp
// Use connection pooling with http_client_config
http_client_config config;
config.set_timeout(std::chrono::seconds(30));
config.set_validate_certificates(true);

http_client client(U("https://api.example.com"), config);
```

```cpp
// Implement retry logic for transient failures
pplx::task<http_response> requestWithRetry(http_client& client, 
    const method& mtd, const std::string& path, int maxRetries = 3) {
    
    return client.request(mtd, U(path))
        .then([&client, mtd, path, maxRetries](pplx::task<http_response> previousTask) {
            try {
                auto response = previousTask.get();
                if (response.status_code() >= 500 && maxRetries > 0) {
                    return pplx::complete_after(std::chrono::seconds(1))
                        .then([&client, mtd, path, maxRetries]() {
                            return requestWithRetry(client, mtd, path, maxRetries - 1);
                        });
                }
                return pplx::task_from_result(response);
            } catch (...) {
                if (maxRetries > 0) {
                    return pplx::complete_after(std::chrono::seconds(1))
                        .then([&client, mtd, path, maxRetries]() {
                            return requestWithRetry(client, mtd, path, maxRetries - 1);
                        });
                }
                throw;
            }
        });
}
```

```cpp
// Use proper error handling patterns
class ApiClient {
private:
    http_client m_client;
    
public:
    pplx::task<json::value> getData(const std::string& endpoint) {
        return m_client.request(methods::GET, U(endpoint))
            .then([](http_response response) {
                if (response.status_code() == status_codes::OK) {
                    return response.extract_json();
                }
                throw http_exception(response.status_code(), 
                    "Request failed with status: " + 
                    std::to_string(response.status_code()));
            });
    }
};
```

```cpp
// Use stream-based operations for large payloads
pplx::task<void> downloadLargeFile(http_client& client, 
    const std::string& url, const std::string& outputPath) {
    
    auto outputStream = std::make_shared<concurrency::streams::ostream>();
    
    return concurrency::streams::fstream::open_ostream(U(outputPath))
        .then([&client, url, outputStream](concurrency::streams::ostream file) {
            *outputStream = file;
            return client.request(methods::GET, U(url));
        })
        .then([outputStream](http_response response) {
            return response.body().read_to_end(outputStream->streambuf());
        })
        .then([outputStream](size_t) {
            return outputStream->close();
        });
}
```