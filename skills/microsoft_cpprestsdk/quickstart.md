# Quickstart

```cpp
// Basic HTTP GET request
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

int main() {
    http_client client(U("https://api.example.com"));
    
    client.request(methods::GET, U("/data"))
        .then([](http_response response) {
            return response.extract_string();
        })
        .then([](std::string body) {
            std::cout << body << std::endl;
        })
        .wait();
    
    return 0;
}
```

```cpp
// HTTP POST with JSON body
#include <cpprest/http_client.h>
#include <cpprest/json.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::json;

int main() {
    http_client client(U("https://api.example.com"));
    
    json::value postData;
    postData[U("name")] = json::value::string(U("John"));
    postData[U("age")] = json::value::number(30);
    
    client.request(methods::POST, U("/users"), postData)
        .then([](http_response response) {
            return response.extract_json();
        })
        .then([](json::value body) {
            std::cout << body.serialize() << std::endl;
        })
        .wait();
    
    return 0;
}
```

```cpp
// File download with stream
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

int main() {
    auto fileStream = std::make_shared<concurrency::streams::ostream>();
    
    concurrency::streams::fstream::open_ostream(U("output.txt"))
        .then([=](concurrency::streams::ostream outFile) {
            *fileStream = outFile;
            http_client client(U("https://example.com"));
            return client.request(methods::GET, U("/file.txt"));
        })
        .then([=](http_response response) {
            return response.body().read_to_end(fileStream->streambuf());
        })
        .then([=](size_t size) {
            std::cout << "Downloaded " << size << " bytes" << std::endl;
        })
        .wait();
    
    return 0;
}
```

```cpp
// HTTP listener (server)
#include <cpprest/http_listener.h>
#include <cpprest/json.h>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

int main() {
    http_listener listener(U("http://localhost:8080/api"));
    
    listener.support(methods::GET, [](http_request request) {
        json::value response;
        response[U("message")] = json::value::string(U("Hello World"));
        request.reply(status_codes::OK, response);
    });
    
    listener.open().wait();
    std::cout << "Server running on http://localhost:8080" << std::endl;
    std::cin.get();
    listener.close().wait();
    
    return 0;
}
```

```cpp
// WebSocket client
#include <cpprest/ws_client.h>

using namespace web;
using namespace web::websockets::client;

int main() {
    websocket_client client;
    
    client.connect(U("wss://echo.example.com")).then([&client]() {
        websocket_outgoing_message msg;
        msg.set_utf8_message("Hello WebSocket");
        return client.send(msg);
    }).then([&client]() {
        return client.receive();
    }).then([](websocket_incoming_message msg) {
        return msg.extract_string();
    }).then([](std::string message) {
        std::cout << "Received: " << message << std::endl;
    }).wait();
    
    client.close().wait();
    return 0;
}
```

```cpp
// Async task composition with error handling
#include <cpprest/http_client.h>
#include <cpprest/json.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

pplx::task<void> fetchUserData(const std::string& userId) {
    http_client client(U("https://api.example.com"));
    
    return client.request(methods::GET, U("/users/") + utility::conversions::to_string_t(userId))
        .then([](http_response response) {
            if (response.status_code() == status_codes::OK) {
                return response.extract_json();
            }
            throw std::runtime_error("Request failed");
        })
        .then([](json::value data) {
            std::cout << "User: " << data.serialize() << std::endl;
        });
}

int main() {
    try {
        fetchUserData("12345").wait();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
```