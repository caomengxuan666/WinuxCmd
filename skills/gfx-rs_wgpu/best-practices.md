# Best Practices

```cpp
// Production-ready patterns for wgpu

#include <webgpu/webgpu.h>
#include <memory>
#include <vector>
#include <unordered_map>

// Pattern 1: RAII wrapper for resources
template<typename T>
class WGPUResource {
    T resource;
public:
    WGPUResource(T res) : resource(res) {}
    ~WGPUResource() { if (resource) release(resource); }
    WGPUResource(const WGPUResource&) = delete;
    WGPUResource& operator=(const WGPUResource&) = delete;
    WGPUResource(WGPUResource&& other) noexcept : resource(other.resource) {
        other.resource = nullptr;
    }
    T get() const { return resource; }
    
private:
    static void release(WGPUBuffer b) { wgpuBufferRelease(b); }
    static void release(WGPUTexture t) { wgpuTextureRelease(t); }
    static void release(WGPURenderPipeline p) { wgpuRenderPipelineRelease(p); }
};

// Pattern 2: Resource pool for reuse
class BufferPool {
    WGPUDevice device;
    std::vector<WGPUBuffer> available;
    std::vector<WGPUBuffer> inUse;
    
public:
    BufferPool(WGPUDevice dev) : device(dev) {}
    
    WGPUBuffer acquire(size_t size, WGPUBufferUsageFlags usage) {
        if (!available.empty()) {
            WGPUBuffer buf = available.back();
            available.pop_back();
            inUse.push_back(buf);
            return buf;
        }
        
        WGPUBufferDescriptor desc = {};
        desc.size = size;
        desc.usage = usage;
        WGPUBuffer buf = wgpuDeviceCreateBuffer(device, &desc);
        inUse.push_back(buf);
        return buf;
    }
    
    void release(WGPUBuffer buf) {
        auto it = std::find(inUse.begin(), inUse.end(), buf);
        if (it != inUse.end()) {
            inUse.erase(it);
            available.push_back(buf);
        }
    }
    
    ~BufferPool() {
        for (auto buf : available) wgpuBufferRelease(buf);
        for (auto buf : inUse) wgpuBufferRelease(buf);
    }
};

// Pattern 3: Async initialization
class WGPUContext {
    WGPUInstance instance;
    WGPUAdapter adapter;
    WGPUDevice device;
    WGPUQueue queue;
    
public:
    static std::future<WGPUContext> createAsync() {
        return std::async(std::launch::async, []() {
            WGPUContext ctx;
            ctx.instance = wgpuCreateInstance(nullptr);
            // ... async adapter and device creation
            return ctx;
        });
    }
};

// Pattern 4: Frame pacing with triple buffering
class FrameManager {
    static const int NUM_FRAMES = 3;
    WGPUSwapChain swapChains[NUM_FRAMES];
    int currentFrame = 0;
    
public:
    void beginFrame() {
        // Wait for previous frame to complete
        wgpuQueueOnSubmittedWorkDone(queue, nullptr, nullptr);
    }
    
    void endFrame() {
        currentFrame = (currentFrame + 1) % NUM_FRAMES;
    }
};

// Pattern 5: Error handling with callbacks
void setupErrorHandling(WGPUDevice device) {
    wgpuDeviceSetUncapturedErrorCallback(device, 
        [](WGPUErrorType type, const char* message, void*) {
            std::cerr << "GPU Error: " << message << std::endl;
            // Log and potentially recover
        }, nullptr);
}

// Pattern 6: Shader compilation caching
class ShaderCache {
    std::unordered_map<std::string, WGPUShaderModule> cache;
    WGPUDevice device;
    
public:
    WGPUShaderModule getOrCreate(const std::string& source) {
        auto it = cache.find(source);
        if (it != cache.end()) return it->second;
        
        WGPUShaderModuleWGSLDescriptor wgslDesc = {};
        wgslDesc.code = source.c_str();
        WGPUShaderModuleDescriptor desc = {};
        desc.nextInChain = &wgslDesc.chain;
        
        WGPUShaderModule module = wgpuDeviceCreateShaderModule(device, &desc);
        cache[source] = module;
        return module;
    }
};
```