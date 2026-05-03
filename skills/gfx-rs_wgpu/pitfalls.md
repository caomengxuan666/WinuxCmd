# Pitfalls

```cpp
// Common mistakes and their corrections

// BAD: Forgetting to set usage flags
WGPUBuffer createBadBuffer(WGPUDevice device) {
    WGPUBufferDescriptor desc = {};
    desc.size = 1024;
    // Missing usage flags!
    return wgpuDeviceCreateBuffer(device, &desc); // Will fail
}

// GOOD: Always specify buffer usage
WGPUBuffer createGoodBuffer(WGPUDevice device) {
    WGPUBufferDescriptor desc = {};
    desc.size = 1024;
    desc.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
    return wgpuDeviceCreateBuffer(device, &desc);
}

// BAD: Using device after release
void badDeviceUsage() {
    WGPUDevice device = /* ... */;
    wgpuDeviceRelease(device);
    WGPUQueue queue = wgpuDeviceGetQueue(device); // Use-after-free!
}

// GOOD: Proper device lifecycle
void goodDeviceUsage() {
    WGPUDevice device = /* ... */;
    WGPUQueue queue = wgpuDeviceGetQueue(device);
    wgpuQueueReference(queue); // Increment ref count
    wgpuDeviceRelease(device);
    // Now queue is safe to use
    wgpuQueueSubmit(queue, 0, nullptr);
    wgpuQueueRelease(queue);
}

// BAD: Modifying pipeline state after creation
void badPipelineModification(WGPURenderPipeline pipeline) {
    // Pipelines are immutable after creation
    // Cannot modify shader or state
}

// GOOD: Create new pipeline for different configurations
WGPURenderPipeline createPipelineWithConfig(WGPUDevice device, 
                                            WGPUPrimitiveTopology topology) {
    WGPURenderPipelineDescriptor desc = {};
    desc.primitive.topology = topology;
    // ... configure other states
    return wgpuDeviceCreateRenderPipeline(device, &desc);
}

// BAD: Incorrect texture format for swap chain
void badSwapChainFormat(WGPUDevice device, WGPUSurface surface) {
    WGPUSwapChainDescriptor desc = {};
    desc.format = WGPUTextureFormat_RGBA8Unorm; // May not be supported
    wgpuDeviceCreateSwapChain(device, surface, &desc);
}

// GOOD: Query supported formats
WGPUTextureFormat getPreferredFormat(WGPUAdapter adapter) {
    // BGRA8Unorm is most widely supported
    return WGPUTextureFormat_BGRA8Unorm;
}

// BAD: Not waiting for GPU completion before buffer mapping
void badBufferMapping(WGPUBuffer buffer) {
    wgpuBufferMapAsync(buffer, WGPUMapMode_Read, 0, 1024,
        [](WGPUBufferMapAsyncStatus status, void* userdata) {
            // May read incomplete data
        }, nullptr);
}

// GOOD: Proper synchronization
void goodBufferMapping(WGPUQueue queue, WGPUBuffer buffer) {
    // Submit work first
    wgpuQueueSubmit(queue, 0, nullptr);
    // Wait for completion
    wgpuQueueOnSubmittedWorkDone(queue, 
        [](WGPUQueueWorkDoneStatus status, void* userdata) {
            // Now safe to map
            wgpuBufferMapAsync(static_cast<WGPUBuffer>(userdata),
                WGPUMapMode_Read, 0, 1024, nullptr, nullptr);
        }, buffer);
}

// BAD: Memory leak from not releasing resources
void badResourceLeak(WGPUDevice device) {
    for (int i = 0; i < 100; i++) {
        WGPUBuffer buffer = wgpuDeviceCreateBuffer(device, &desc);
        // Never released!
    }
}

// GOOD: RAII wrapper or explicit release
void goodResourceManagement(WGPUDevice device) {
    std::vector<WGPUBuffer> buffers;
    for (int i = 0; i < 100; i++) {
        buffers.push_back(wgpuDeviceCreateBuffer(device, &desc));
    }
    // Release all buffers
    for (auto buffer : buffers) {
        wgpuBufferRelease(buffer);
    }
}
```