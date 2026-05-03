# Safety

```cpp
// CRITICAL: These conditions must NEVER occur

// RED LINE 1: Never access GPU memory while mapped
// BAD: Using buffer while mapped
void badMappedAccess(WGPUBuffer buffer) {
    wgpuBufferMapAsync(buffer, WGPUMapMode_Write, 0, 1024, nullptr, nullptr);
    // Buffer is mapped, but we try to use it in a draw call
    // This will cause undefined behavior!
}

// GOOD: Unmap before GPU usage
void goodMappedAccess(WGPUBuffer buffer) {
    wgpuBufferMapAsync(buffer, WGPUMapMode_Write, 0, 1024, 
        [](WGPUBufferMapAsyncStatus status, void* userdata) {
            WGPUBuffer buf = static_cast<WGPUBuffer>(userdata);
            void* data = wgpuBufferGetMappedRange(buf, 0, 1024);
            // Write data
            memcpy(data, "hello", 5);
            wgpuBufferUnmap(buf); // Must unmap before GPU uses it
        }, buffer);
}

// RED LINE 2: Never submit commands after device destruction
// BAD: Submitting after device lost
void badDeviceLost(WGPUDevice device) {
    wgpuDeviceDestroy(device);
    WGPUQueue queue = wgpuDeviceGetQueue(device); // Device is destroyed!
}

// GOOD: Check device state
void goodDeviceCheck(WGPUDevice device) {
    if (wgpuDeviceGetLostCallback(device) != nullptr) {
        // Device is still valid
        WGPUQueue queue = wgpuDeviceGetQueue(device);
    }
}

// RED LINE 3: Never use texture views after texture destruction
// BAD: Texture view dangling
void badTextureView(WGPUTexture texture) {
    WGPUTextureView view = wgpuTextureCreateView(texture, nullptr);
    wgpuTextureRelease(texture);
    // view now points to destroyed texture
    wgpuTextureViewRelease(view); // Undefined behavior!
}

// GOOD: Keep texture alive while view exists
void goodTextureView(WGPUTexture texture) {
    WGPUTextureView view = wgpuTextureCreateView(texture, nullptr);
    wgpuTextureReference(texture); // Increment ref count
    wgpuTextureRelease(texture);
    // view is still valid
    wgpuTextureViewRelease(view);
    wgpuTextureRelease(texture);
}

// RED LINE 4: Never create resources with invalid usage combinations
// BAD: Conflicting usage flags
void badUsageFlags(WGPUDevice device) {
    WGPUBufferDescriptor desc = {};
    desc.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_Index;
    // Vertex and index are mutually exclusive!
    wgpuDeviceCreateBuffer(device, &desc); // Will fail
}

// GOOD: Separate buffers for different purposes
void goodUsageFlags(WGPUDevice device) {
    WGPUBufferDescriptor vertexDesc = {};
    vertexDesc.usage = WGPUBufferUsage_Vertex;
    WGPUBuffer vertexBuffer = wgpuDeviceCreateBuffer(device, &vertexDesc);
    
    WGPUBufferDescriptor indexDesc = {};
    indexDesc.usage = WGPUBufferUsage_Index;
    WGPUBuffer indexBuffer = wgpuDeviceCreateBuffer(device, &indexDesc);
}

// RED LINE 5: Never call wgpuQueueSubmit from multiple threads simultaneously
// BAD: Concurrent submits
void badConcurrentSubmit(WGPUQueue queue) {
    std::thread t1([&]() { wgpuQueueSubmit(queue, 0, nullptr); });
    std::thread t2([&]() { wgpuQueueSubmit(queue, 0, nullptr); });
    t1.join();
    t2.join(); // Race condition!
}

// GOOD: Serialize queue submissions
void goodConcurrentSubmit(WGPUQueue queue) {
    std::mutex mtx;
    std::thread t1([&]() { 
        std::lock_guard<std::mutex> lock(mtx);
        wgpuQueueSubmit(queue, 0, nullptr); 
    });
    std::thread t2([&]() { 
        std::lock_guard<std::mutex> lock(mtx);
        wgpuQueueSubmit(queue, 0, nullptr); 
    });
    t1.join();
    t2.join();
}
```