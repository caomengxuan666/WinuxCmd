# Quickstart

```cpp
#include <zmq.hpp>
#include <string>
#include <iostream>

// 1. Basic push/pull pattern
void basic_push_pull() {
    zmq::context_t ctx;
    zmq::socket_t pusher(ctx, zmq::socket_type::push);
    zmq::socket_t puller(ctx, zmq::socket_type::pull);
    
    pusher.bind("inproc://example");
    puller.connect("inproc://example");
    
    pusher.send(zmq::str_buffer("Hello"), zmq::send_flags::none);
    
    zmq::message_t msg;
    puller.recv(msg, zmq::recv_flags::none);
    std::cout << msg.to_string() << std::endl;
}

// 2. Request/reply pattern
void request_reply() {
    zmq::context_t ctx;
    zmq::socket_t req(ctx, zmq::socket_type::req);
    zmq::socket_t rep(ctx, zmq::socket_type::rep);
    
    req.bind("inproc://reqrep");
    rep.connect("inproc://reqrep");
    
    req.send(zmq::str_buffer("Ping"), zmq::send_flags::none);
    
    zmq::message_t reply;
    rep.recv(reply, zmq::recv_flags::none);
    rep.send(zmq::str_buffer("Pong"), zmq::send_flags::none);
    
    req.recv(reply, zmq::recv_flags::none);
    std::cout << reply.to_string() << std::endl;
}

// 3. Publisher/subscriber pattern
void pub_sub() {
    zmq::context_t ctx;
    zmq::socket_t pub(ctx, zmq::socket_type::pub);
    zmq::socket_t sub(ctx, zmq::socket_type::sub);
    
    pub.bind("inproc://pubsub");
    sub.connect("inproc://pubsub");
    sub.set(zmq::sockopt::subscribe, "topic");
    
    pub.send(zmq::str_buffer("topic Hello"), zmq::send_flags::none);
    
    zmq::message_t msg;
    sub.recv(msg, zmq::recv_flags::none);
    std::cout << msg.to_string() << std::endl;
}

// 4. Multi-part message sending
void multipart_message() {
    zmq::context_t ctx;
    zmq::socket_t sender(ctx, zmq::socket_type::push);
    zmq::socket_t receiver(ctx, zmq::socket_type::pull);
    
    sender.bind("inproc://multi");
    receiver.connect("inproc://multi");
    
    // Send multi-part message
    sender.send(zmq::str_buffer("Part1"), zmq::send_flags::sndmore);
    sender.send(zmq::str_buffer("Part2"), zmq::send_flags::sndmore);
    sender.send(zmq::str_buffer("Part3"), zmq::send_flags::none);
    
    // Receive multi-part message
    zmq::message_t msg;
    receiver.recv(msg, zmq::recv_flags::none);
    std::cout << msg.to_string() << std::endl;
}

// 5. Using zmq_addon.hpp for multipart
#include <zmq_addon.hpp>
void addon_multipart() {
    zmq::context_t ctx;
    zmq::socket_t sock1(ctx, zmq::socket_type::push);
    zmq::socket_t sock2(ctx, zmq::socket_type::pull);
    
    sock1.bind("inproc://addon");
    sock2.connect("inproc://addon");
    
    std::array<zmq::const_buffer, 2> send_msgs = {
        zmq::str_buffer("foo"),
        zmq::str_buffer("bar")
    };
    zmq::send_multipart(sock1, send_msgs);
    
    std::vector<zmq::message_t> recv_msgs;
    zmq::recv_multipart(sock2, std::back_inserter(recv_msgs));
}

// 6. TCP with wildcard port
void tcp_wildcard() {
    zmq::context_t ctx;
    zmq::socket_t sock(ctx, zmq::socket_type::push);
    sock.bind("tcp://127.0.0.1:*");
    
    std::string endpoint = sock.get(zmq::sockopt::last_endpoint);
    std::cout << "Bound to: " << endpoint << std::endl;
}

// 7. Non-blocking send
void nonblocking_send() {
    zmq::context_t ctx;
    zmq::socket_t sock(ctx, zmq::socket_type::push);
    sock.bind("inproc://nonblock");
    
    zmq::send_result_t result = sock.send(
        zmq::str_buffer("test"), 
        zmq::send_flags::dontwait
    );
    if (!result) {
        std::cout << "Send would block" << std::endl;
    }
}

// 8. Polling multiple sockets
void polling_example() {
    zmq::context_t ctx;
    zmq::socket_t sock1(ctx, zmq::socket_type::pull);
    zmq::socket_t sock2(ctx, zmq::socket_type::pull);
    
    sock1.bind("inproc://poll1");
    sock2.bind("inproc://poll2");
    
    std::vector<zmq::pollitem_t> items = {
        { sock1, 0, ZMQ_POLLIN, 0 },
        { sock2, 0, ZMQ_POLLIN, 0 }
    };
    
    zmq::poll(items, std::chrono::milliseconds(100));
}
```