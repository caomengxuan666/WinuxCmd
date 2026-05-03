# Lifecycle

```cpp
// Resource lifecycle management in wgpu

#include <webgpu/webgpu.h>

// Construction: Instance -> Adapter -> Device -> Resources
WGPUInstance createInstance() {
    WGPUInstanceDescriptor desc = {};
    return wgpuCreateInstance(&desc);
}

WGPUAdapter requestAdapter(WGPUInstance instance) {
    WGPURequestAdapterOptions options = {};
    options.powerPreference = WGPUPowerPreference_HighPerformance;
    
    WGPUAdapter adapter = nullptr;
    wgpuInstanceRequestAdapter(instance, &options,
        [](WGPURequestAdapterStatus status, WGPUAdapter a, 
           const char* msg, void* userdata) {
            *static_cast<WGPUAdapter*>(userdata) = a;
        }, &adapter);
    return adapter;
}

// Destruction: Reverse order of creation
void destroyResources(WGPUDevice device) {
    // 1. Destroy device-specific resources
    // 2. Release device
    wgpuDeviceRelease(device);
    // 3. Release adapter
    // 4. Release instance
}

// Move semantics: wgpu objects are reference-counted
void moveSemantics() {
    WGPUDevice device1 = /* ... */;
    WGPUDevice device2 = device1; // Shallow copy, same ref count
    
    wgpuDeviceReference(device1); // Increment ref count
    wgpuDeviceRelease(device1);   // Decrement ref count
    // device2 still valid
}

// Resource management: Buffers
class BufferManager {
    WGPUDevice device;
    std::vector<WGPUBuffer> buffers;
    
public:
    WGPUBuffer createBuffer(size_t size, WGPUBufferUsageFlags usage) {
        WGPUBufferDescriptor desc = {};
        desc.size = size;
        desc.usage = usage;
        desc.mappedAtCreation = false;
        
        WGPUBuffer buffer = wgpuDeviceCreateBuffer(device, &desc);
        buffers.push_back(buffer);
        return buffer;
    }
    
    void destroyBuffer(WGPUBuffer buffer) {
        auto it = std::find(buffers.begin(), buffers.end(), buffer);
        if (it != buffers.end()) {
            wgpuBufferDestroy(*it); // Immediately free GPU memory
            wgpuBufferRelease(*it); // Release reference
            buffers.erase(it);
        }
    }
    
    ~BufferManager() {
        for (auto buffer : buffers) {
            wgpuBufferDestroy(buffer);
            wgpuBufferRelease(buffer);
        }
    }
};

// Texture lifecycle
void textureLifecycle(WGPUDevice device) {
    WGPUTextureDescriptor desc = {};
    desc.size = {1024, 1024, 1};
    desc.format = WGPUTextureFormat_RGBA8Unorm;
    desc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
    
    WGPUTexture texture = wgpuDeviceCreateTexture(device, &desc);
    WGPUTextureView view = wgpuTextureCreateView(texture, nullptr);
    
    // Use texture...
    
    // Cleanup: destroy view first, then texture
    wgpuTextureViewRelease(view);
    wgpuTextureDestroy(texture);
    wgpuTextureRelease(texture);
}

// Pipeline lifecycle: Immutable after creation
void pipelineLifecycle(WGPUDevice device) {
    WGPURenderPipeline pipeline = /* create pipeline */;
    
    // Pipeline can be used multiple times
    // Cannot modify after creation
    
    // Destroy when no longer needed
    wgpuRenderPipelineRelease(pipeline);
}

// Swap chain lifecycle: Recreate on resize
void swapChainLifecycle(WGPUDevice device, WGPUSurface surface) {
    int width = 800, height = 600;
    
    WGPUSwapChain swapChain = nullptr;
    
    auto recreateSwapChain = [&]() {
        if (swapChain) {
            wgpuSwapChainRelease(swapChain);
        }
        
        WGPUSwapChainDescriptor desc = {};
        desc.width = width;
        desc.height = height;
        desc.format = WGPUTextureFormat_BGRA8Unorm;
        desc.usage = WGPUTextureUsage_RenderAttachment;
        desc.presentMode = WGPUPresentMode_Fifo;
        
        swapChain = wgpuDeviceCreateSwapChain(device, surface, &desc);
    };
    
    // On resize:
    // width = newWidth;
    // height = newHeight;
    // recreateSwapChain();
}
```