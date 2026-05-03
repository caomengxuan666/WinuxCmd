# Safety

### Red Line 1: NEVER parse untrusted data without size limits
```cpp
// BAD: No size limit on untrusted input
std::string untrusted_data = ReceiveFromNetwork();
MyMessage msg;
msg.ParseFromString(untrusted_data);  // Could be gigabytes!

// GOOD: Set reasonable limits
std::string untrusted_data = ReceiveFromNetwork();
if (untrusted_data.size() > 10 * 1024 * 1024) {  // 10MB limit
    throw std::runtime_error("Message too large");
}
MyMessage msg;
if (!msg.ParseFromString(untrusted_data)) {
    throw std::runtime_error("Parse failed");
}
```

### Red Line 2: NEVER use mutable accessors on shared messages
```cpp
// BAD: Concurrent modification
MyMessage msg;
// Thread 1:
msg.mutable_person()->set_name("Alice");
// Thread 2 (concurrent):
msg.mutable_person()->set_age(30);  // Data race!

// GOOD: Use separate copies or mutex
std::mutex mtx;
MyMessage msg;
// Thread 1:
{
    std::lock_guard<std::mutex> lock(mtx);
    msg.mutable_person()->set_name("Alice");
}
// Thread 2:
{
    std::lock_guard<std::mutex> lock(mtx);
    msg.mutable_person()->set_age(30);
}
```

### Red Line 3: NEVER delete memory returned by mutable accessors
```cpp
// BAD: Deleting protobuf-owned memory
Person* p = message.mutable_person();
delete p;  // CRASH! Double free or heap corruption

// GOOD: Just use the pointer
Person* p = message.mutable_person();
p->set_name("Alice");
// Don't delete - protobuf manages it
```

### Red Line 4: NEVER ignore parse failures
```cpp
// BAD: Ignoring parse return value
MyMessage msg;
msg.ParseFromString(data);  // Returns false, but ignored
ProcessMessage(msg);  // May process corrupted data

// GOOD: Always check and handle errors
MyMessage msg;
if (!msg.ParseFromString(data)) {
    LogError("Failed to parse message");
    return;
}
ProcessMessage(msg);
```

### Red Line 5: NEVER modify proto files without updating field numbers
```cpp
// BAD: Reusing field numbers after deletion
message OldVersion {
  string name = 1;
  int32 age = 2;
}
// Later version:
message NewVersion {
  string email = 1;  // SAME NUMBER as old 'name'!
  int32 age = 2;
}
// This causes silent data corruption!

// GOOD: Reserve old field numbers
message SafeVersion {
  reserved 1;  // Previously used by 'name'
  string email = 2;
  int32 age = 3;
}
```