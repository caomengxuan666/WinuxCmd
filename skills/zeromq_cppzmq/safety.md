# Safety


### Red Line 1: Never access socket after context destruction
```cpp
// BAD: Context destroyed before socket
zmq::context_t* ctx = new zmq::context_t();
zmq::socket_t* sock = new zmq::socket_t(*ctx, zmq::socket_type::push);
delete ctx; // Context destroyed
sock->send(zmq::str_buffer("test"), zmq::send_flags::none); // CRASH!

// GOOD: Ensure context outlives all sockets
zmq::context_t ctx;
{
    zmq::socket_t sock(ctx, zmq::socket_type::push);
    sock.send(zmq::str_buffer("test"), zmq::send_flags::none);
} // sock destroyed first
// ctx destroyed last
```

### Red Line 2: Never send on a closed socket
```cpp
// BAD: Sending after close
zmq::context_t ctx;
zmq::socket_t sock(ctx, zmq::socket_type::push);
sock.close();
sock.send(zmq::str_buffer("test"), zmq::send_flags::none); // Undefined behavior!

// GOOD: Check socket state before operations
zmq::context_t ctx;
zmq::socket_t sock(ctx, zmq::socket_type::push);
if (sock.handle() != nullptr) {
    sock.send(zmq::str_buffer("test"), zmq::send_flags::none);
}
```

### Red Line 3: Never use REQ socket without receiving reply
```cpp
// BAD: REQ socket sending without receiving
zmq::context_t ctx;
zmq::socket_t req(ctx, zmq::socket_type::req);
req.connect("inproc://test");
req.send(zmq::str_buffer("req1"), zmq::send_flags::none);
req.send(zmq::str_buffer("req2"), zmq::send_flags::none); // Throws EFSM!

// GOOD: Always alternate send/recv on REQ
zmq::context_t ctx;
zmq::socket_t req(ctx, zmq::socket_type::req);
req.connect("inproc://test");
req.send(zmq::str_buffer("req1"), zmq::send_flags::none);
zmq::message_t reply;
req.recv(reply, zmq::recv_flags::none);
req.send(zmq::str_buffer("req2"), zmq::send_flags::none);
```

### Red Line 4: Never use message_t after moving from it
```cpp
// BAD: Using moved-from message
zmq::message_t msg;
msg.init_size(100);
zmq::message_t moved = std::move(msg);
memcpy(msg.data(), "data", 4); // Undefined behavior!

// GOOD: Reinitialize after move
zmq::message_t msg;
msg.init_size(100);
zmq::message_t moved = std::move(msg);
msg = zmq::message_t(); // Reinitialize
msg.init_size(50);
memcpy(msg.data(), "data", 4); // OK
```

### Red Line 5: Never mix contexts for inproc communication
```cpp
// BAD: inproc across different contexts
zmq::context_t ctx1;
zmq::context_t ctx2;
zmq::socket_t pub(ctx1, zmq::socket_type::pub);
zmq::socket_t sub(ctx2, zmq::socket_type::sub);
pub.bind("inproc://test");
sub.connect("inproc://test"); // Will silently fail!

// GOOD: Same context for inproc
zmq::context_t ctx;
zmq::socket_t pub(ctx, zmq::socket_type::pub);
zmq::socket_t sub(ctx, zmq::socket_type::sub);
pub.bind("inproc://test");
sub.connect("inproc://test");
```