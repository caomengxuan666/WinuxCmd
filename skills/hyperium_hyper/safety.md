# Safety

RED LINE 1: Never use a client after it has been moved
```cpp
// BAD: Using moved-from client
hyper::client::Client client1;
hyper::client::Client client2 = std::move(client1);

auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com")
    .build();

auto response = client1.request(req).get(); // UNDEFINED BEHAVIOR
```

```cpp
// GOOD: Only use valid clients
hyper::client::Client client1;
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com")
    .build();

auto response = client1.request(req).get(); // Safe

hyper::client::Client client2 = std::move(client1);
// client1 is now in moved-from state, do not use
```

RED LINE 2: Never access response body after moving it
```cpp
// BAD: Accessing moved body
auto response = client.request(req).get();
auto body = response.into_body();
auto moved_body = std::move(body);
auto data = body.data(); // UNDEFINED BEHAVIOR
```

```cpp
// GOOD: Only access body from valid object
auto response = client.request(req).get();
auto body = response.into_body();
auto data = body.data(); // Safe
```

RED LINE 3: Never call request methods on a destroyed client
```cpp
// BAD: Using client after destruction
hyper::client::Client* client_ptr = new hyper::client::Client();
delete client_ptr;

auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com")
    .build();

auto response = client_ptr->request(req).get(); // UNDEFINED BEHAVIOR
```

```cpp
// GOOD: Ensure client lifetime exceeds request
hyper::client::Client client;
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com")
    .build();

auto response = client.request(req).get(); // Safe
```

RED LINE 4: Never modify request headers while request is in flight
```cpp
// BAD: Modifying request during async operation
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com")
    .build();

auto future = client.request_async(req);
req.set_header("X-Custom", "value"); // RACE CONDITION
```

```cpp
// GOOD: Complete request before sending
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com")
    .header("X-Custom", "value")
    .build();

auto future = client.request_async(req); // Safe
```

RED LINE 5: Never use TLS without verification in production
```cpp
// BAD: Disabling TLS verification
hyper::tls::Config tls_config;
tls_config.set_verify(false); // INSECURE

hyper::client::Config config;
config.set_tls_config(tls_config);
hyper::client::Client client(config);
```

```cpp
// GOOD: Always verify TLS certificates
hyper::tls::Config tls_config;
tls_config.set_verify(true);
tls_config.set_ca_file("/etc/ssl/certs/ca-certificates.crt");

hyper::client::Config config;
config.set_tls_config(tls_config);
hyper::client::Client client(config);
```