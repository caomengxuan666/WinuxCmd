# Threading

```cpp
// Thread safety considerations in wgpu

#include <webgpu/webgpu.h>
#include <mutex>
#include <thread>
#include <atomic>

// Thread safety guarantees:
// - WGPUInstance: Fully thread-safe
// - WGPUAdapter: Fully thread-safe
// - WGPUDevice: Fully thread-safe
// - WGPUQueue: NOT thread-safe for submission
// - WGPUCommandEncoder: NOT thread-safe
// - WGPUBuffer: Thread-safe for mapping/unmapping
// - WGPUTexture: Thread-safe for operations

// 1. Queue submission must be serialized
class ThreadSafeQueue {
    WGPUQueue queue;
    std::mutex mtx;
    
public:
    void submit(uint32_t count, const WGPUCommandBuffer* commands) {
        std::lock_guard<std::mutex> lock(mtx);
        wgpuQueueSubmit(queue, count, commands);
    }
    
    void onSubmittedWorkDone(WGPUQueueWorkDoneCallback callback, void* userdata) {
        std::lock_guard<std::mutex> lock(mtx);
        wgpuQueueOnSubmittedWorkDone(queue, callback, userdata);
    }
};

// 2. Command encoding can be parallelized
void parallelCommandEncoding(WGPUDevice device) {
    const int NUM_THREADS = 4;
    std::vector<WGPUCommandEncoder> encoders(NUM_THREADS);
    std::vector<std::thread> threads;
    
    // Create encoders on main thread
    for (int i = 0; i < NUM_THREADS; i++) {
        encoders[i] = wgpuDeviceCreateCommandEncoder(device, nullptr);
    }
    
    // Parallel encoding
    for (int i = 0; i < NUM_THREADS; i++) {
        threads.emplace_back([&encoders, i]() {
            // Each thread works on its own encoder
            WGPURenderPassEncoder pass = /* begin pass on encoders[i] */;
            // Record commands
            wgpuRenderPassEncoderEnd(pass);
        });
    }
    
    // Wait for all threads
    for (auto& t : threads) t.join();
    
    // Collect command buffers on main thread
    std::vector<WGPUCommandBuffer> commandBuffers(NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; i++) {
        commandBuffers[i] = wgpuCommandEncoderFinish(encoders[i], nullptr);
    }
    
    // Submit all at once
    WGPUQueue queue = wgpuDeviceGetQueue(device);
    wgpuQueueSubmit(queue, NUM_THREADS, commandBuffers.data());
}

// 3. Buffer mapping from multiple threads
class ThreadSafeBuffer {
    WGPUBuffer buffer;
    std::mutex mtx;
    std::atomic<bool> mapped{false};
    
public:
    void mapAsync(WGPUMapMode mode, size_t offset, size_t size,
                  WGPUBufferMapCallback callback, void* userdata) {
        std::lock_guard<std::mutex> lock(mtx);
        if (!mapped) {
            wgpuBufferMapAsync(buffer, mode, offset, size, callback, userdata);
            mapped = true;
        }
    }
    
    void unmap() {
        std::lock_guard<std::mutex> lock(mtx);
        if (mapped) {
            wgpuBufferUnmap(buffer);
            mapped = false;
        }
    }
};

// 4. Device creation from worker thread
std::future<WGPUDevice> createDeviceAsync(WGPUAdapter adapter) {
    return std::async(std::launch::async, [adapter]() {
        WGPUDevice device = nullptr;
        WGPUDeviceDescriptor desc = {};
        
        wgpuAdapterRequestDevice(adapter, &desc,
            [](WGPURequestDeviceStatus status, WGPUDevice d,
               const char* message, void* userdata) {
                *static_cast<WGPUDevice*>(userdata) = d;
            }, &device);
        
        return device;
    });
}

// 5. Resource creation from multiple threads
class ParallelResourceCreator {
    WGPUDevice device;
    std::mutex mtx;
    
public:
    WGPUBuffer createBuffer(size_t size, WGPUBufferUsageFlags usage) {
        WGPUBufferDescriptor desc = {};
        desc.size = size;
        desc.usage = usage;
        
        // Device operations are thread-safe
        return wgpuDeviceCreateBuffer(device, &desc);
    }
    
    WGPUTexture createTexture(const WGPUTextureDescriptor& desc) {
        return wgpuDeviceCreateTexture(device, &desc);
    }
};

// 6. Proper synchronization for readback
void threadedReadback(WGPUQueue queue, WGPUBuffer readbackBuffer) {
    std::atomic<bool> done{false};
    
    // Submit work
    wgpuQueueSubmit(queue, 0, nullptr);
    
    // Wait asynchronously
    wgpuQueueOnSubmittedWorkDone(queue, 
        [&done](WGPUQueueWorkDoneStatus status, void*) {
            done = true;
        }, nullptr);
    
    // Worker thread can poll
    std::thread worker([&done, readbackBuffer]() {
        while (!done) {
            std::this_thread::yield();
        }
        
        // Now safe to map
        wgpuBufferMapAsync(readbackBuffer, WGPUMapMode_Read, 0, 1024,
            [](WGPUBufferMapAsyncStatus status, void* userdata) {
                // Process data
            }, nullptr);
    });
    
    worker.detach();
}
```