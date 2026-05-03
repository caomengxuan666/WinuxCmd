# Performance

```cpp
// Performance optimization techniques for wgpu

#include <webgpu/webgpu.h>
#include <chrono>

// 1. Buffer allocation patterns
// BAD: Frequent small allocations
void badAllocation(WGPUDevice device) {
    for (int i = 0; i < 1000; i++) {
        WGPUBufferDescriptor desc = {};
        desc.size = 64; // Small buffer
        desc.usage = WGPUBufferUsage_Uniform;
        WGPUBuffer buffer = wgpuDeviceCreateBuffer(device, &desc);
        // Use and release immediately
        wgpuBufferRelease(buffer);
    }
}

// GOOD: Pool large buffers and suballocate
class BufferAllocator {
    static const size_t POOL_SIZE = 64 * 1024 * 1024; // 64MB
    WGPUBuffer pool;
    size_t offset = 0;
    
public:
    BufferAllocator(WGPUDevice device) {
        WGPUBufferDescriptor desc = {};
        desc.size = POOL_SIZE;
        desc.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
        pool = wgpuDeviceCreateBuffer(device, &desc);
    }
    
    size_t allocate(size_t size) {
        size_t aligned = (size + 255) & ~255; // Align to 256 bytes
        size_t current = offset;
        offset += aligned;
        return current;
    }
};

// 2. Command buffer batching
// BAD: Many small command buffers
void badCommandBatching(WGPUQueue queue) {
    for (int i = 0; i < 100; i++) {
        WGPUCommandEncoder encoder = /* create encoder */;
        // Record small amount of work
        WGPUCommandBuffer commands = wgpuCommandEncoderFinish(encoder, nullptr);
        wgpuQueueSubmit(queue, 1, &commands);
        wgpuCommandBufferRelease(commands);
    }
}

// GOOD: Batch multiple operations
void goodCommandBatching(WGPUQueue queue) {
    WGPUCommandEncoder encoder = /* create encoder */;
    for (int i = 0; i < 100; i++) {
        // Record all work into one encoder
    }
    WGPUCommandBuffer commands = wgpuCommandEncoderFinish(encoder, nullptr);
    wgpuQueueSubmit(queue, 1, &commands);
    wgpuCommandBufferRelease(commands);
}

// 3. Texture upload optimization
// BAD: Synchronous upload
void badTextureUpload(WGPUDevice device, WGPUQueue queue) {
    WGPUTexture texture = /* create texture */;
    // Map buffer, copy data, unmap - all synchronous
    // Blocks GPU pipeline
}

// GOOD: Use staging buffer pipeline
void goodTextureUpload(WGPUDevice device, WGPUQueue queue) {
    // Create staging buffer
    WGPUBuffer staging = /* create staging buffer */;
    // Map and fill staging buffer
    // Copy staging to texture via command encoder
    // Submit and forget
}

// 4. Pipeline state object caching
class PipelineCache {
    std::unordered_map<size_t, WGPURenderPipeline> cache;
    
public:
    WGPURenderPipeline getOrCreate(WGPUDevice device, 
                                    const WGPURenderPipelineDescriptor& desc) {
        size_t hash = hashDescriptor(desc);
        auto it = cache.find(hash);
        if (it != cache.end()) return it->second;
        
        WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(device, &desc);
        cache[hash] = pipeline;
        return pipeline;
    }
    
private:
    size_t hashDescriptor(const WGPURenderPipelineDescriptor& desc) {
        // Implement hash based on pipeline state
        return 0;
    }
};

// 5. Minimize state changes
void optimizeStateChanges(WGPURenderPassEncoder pass) {
    // BAD: Changing state frequently
    for (auto& mesh : meshes) {
        wgpuRenderPassEncoderSetPipeline(pass, mesh.pipeline);
        wgpuRenderPassEncoderSetVertexBuffer(pass, 0, mesh.vertexBuffer, 0, 0);
        wgpuRenderPassEncoderDraw(pass, mesh.vertexCount, 1, 0, 0);
    }
    
    // GOOD: Sort by pipeline
    std::sort(meshes.begin(), meshes.end(), 
              [](auto& a, auto& b) { return a.pipeline < b.pipeline; });
    
    WGPURenderPipeline currentPipeline = nullptr;
    for (auto& mesh : meshes) {
        if (mesh.pipeline != currentPipeline) {
            wgpuRenderPassEncoderSetPipeline(pass, mesh.pipeline);
            currentPipeline = mesh.pipeline;
        }
        wgpuRenderPassEncoderSetVertexBuffer(pass, 0, mesh.vertexBuffer, 0, 0);
        wgpuRenderPassEncoderDraw(pass, mesh.vertexCount, 1, 0, 0);
    }
}

// 6. Use indirect drawing for many objects
void indirectDrawing(WGPUDevice device, WGPUQueue queue) {
    // Create indirect buffer with draw commands
    WGPUBuffer indirectBuffer = /* create buffer with DrawIndexedIndirect commands */;
    
    // Single draw call for many objects
    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, nullptr);
    WGPURenderPassEncoder pass = /* begin pass */;
    wgpuRenderPassEncoderDrawIndirect(pass, indirectBuffer, 0);
    wgpuRenderPassEncoderEnd(pass);
}
```