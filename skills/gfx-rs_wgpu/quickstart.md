# Quickstart

```cpp
// Basic wgpu initialization and triangle rendering
#include <webgpu/webgpu.h>
#include <iostream>
#include <vector>

// Pattern 1: Create instance and adapter
void createAdapter() {
    WGPUInstance instance = wgpuCreateInstance(nullptr);
    WGPURequestAdapterOptions options = {};
    options.powerPreference = WGPUPowerPreference_HighPerformance;
    
    WGPUAdapter adapter = nullptr;
    wgpuInstanceRequestAdapter(instance, &options, 
        [](WGPURequestAdapterStatus status, WGPUAdapter adapter, 
           const char* message, void* userdata) {
            *static_cast<WGPUAdapter*>(userdata) = adapter;
        }, &adapter);
    
    wgpuInstanceRelease(instance);
}

// Pattern 2: Create device and queue
void createDevice(WGPUAdapter adapter) {
    WGPUDeviceDescriptor desc = {};
    desc.label = "Main Device";
    
    WGPUDevice device = nullptr;
    wgpuAdapterRequestDevice(adapter, &desc,
        [](WGPURequestDeviceStatus status, WGPUDevice device,
           const char* message, void* userdata) {
            *static_cast<WGPUDevice*>(userdata) = device;
        }, &device);
    
    WGPUQueue queue = wgpuDeviceGetQueue(device);
}

// Pattern 3: Create shader module
WGPUShaderModule createShader(WGPUDevice device) {
    const char* shaderCode = R"(
        @vertex fn vs(@builtin(vertex_index) in: u32) -> @builtin(position) vec4f {
            var pos = array(vec2f(0.0, 0.5), vec2f(-0.5, -0.5), vec2f(0.5, -0.5));
            return vec4f(pos[in], 0.0, 1.0);
        }
        @fragment fn fs() -> @location(0) vec4f {
            return vec4f(1.0, 0.0, 0.0, 1.0);
        }
    )";
    
    WGPUShaderModuleWGSLDescriptor wgslDesc = {};
    wgslDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
    wgslDesc.code = shaderCode;
    
    WGPUShaderModuleDescriptor desc = {};
    desc.nextInChain = &wgslDesc.chain;
    return wgpuDeviceCreateShaderModule(device, &desc);
}

// Pattern 4: Create render pipeline
WGPURenderPipeline createPipeline(WGPUDevice device, WGPUShaderModule shader) {
    WGPURenderPipelineDescriptor desc = {};
    
    // Vertex state
    desc.vertex.module = shader;
    desc.vertex.entryPoint = "vs";
    
    // Fragment state
    WGPUBlendState blend = {};
    blend.color.srcFactor = WGPUBlendFactor_SrcAlpha;
    blend.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
    blend.color.operation = WGPUBlendOperation_Add;
    blend.alpha = blend.color;
    
    WGPUColorTargetState colorTarget = {};
    colorTarget.format = WGPUTextureFormat_BGRA8Unorm;
    colorTarget.blend = &blend;
    colorTarget.writeMask = WGPUColorWriteMask_All;
    
    WGPUFragmentState fragment = {};
    fragment.module = shader;
    fragment.entryPoint = "fs";
    fragment.targetCount = 1;
    fragment.targets = &colorTarget;
    desc.fragment = &fragment;
    
    // Other states
    desc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    desc.multisample.count = 1;
    
    return wgpuDeviceCreateRenderPipeline(device, &desc);
}

// Pattern 5: Create swap chain
WGPUSwapChain createSwapChain(WGPUDevice device, WGPUSurface surface, 
                              int width, int height) {
    WGPUSwapChainDescriptor desc = {};
    desc.width = width;
    desc.height = height;
    desc.format = WGPUTextureFormat_BGRA8Unorm;
    desc.usage = WGPUTextureUsage_RenderAttachment;
    desc.presentMode = WGPUPresentMode_Fifo;
    
    return wgpuDeviceCreateSwapChain(device, surface, &desc);
}

// Pattern 6: Submit render commands
void renderFrame(WGPUQueue queue, WGPUTextureView view, 
                 WGPURenderPipeline pipeline) {
    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(
        wgpuQueueGetDevice(queue), nullptr);
    
    WGPURenderPassColorAttachment attachment = {};
    attachment.view = view;
    attachment.loadOp = WGPULoadOp_Clear;
    attachment.storeOp = WGPUStoreOp_Store;
    attachment.clearValue = {0.0, 0.0, 0.0, 1.0};
    
    WGPURenderPassDescriptor passDesc = {};
    passDesc.colorAttachmentCount = 1;
    passDesc.colorAttachments = &attachment;
    
    WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(
        encoder, &passDesc);
    wgpuRenderPassEncoderSetPipeline(pass, pipeline);
    wgpuRenderPassEncoderDraw(pass, 3, 1, 0, 0);
    wgpuRenderPassEncoderEnd(pass);
    
    WGPUCommandBuffer commands = wgpuCommandEncoderFinish(encoder, nullptr);
    wgpuQueueSubmit(queue, 1, &commands);
    
    wgpuCommandBufferRelease(commands);
    wgpuRenderPassEncoderRelease(pass);
    wgpuCommandEncoderRelease(encoder);
}

// Pattern 7: Create buffer
WGPUBuffer createBuffer(WGPUDevice device, const void* data, size_t size, 
                        WGPUBufferUsageFlags usage) {
    WGPUBufferDescriptor desc = {};
    desc.size = size;
    desc.usage = usage | WGPUBufferUsage_CopyDst;
    desc.mappedAtCreation = true;
    
    WGPUBuffer buffer = wgpuDeviceCreateBuffer(device, &desc);
    void* mapped = wgpuBufferGetMappedRange(buffer, 0, size);
    memcpy(mapped, data, size);
    wgpuBufferUnmap(buffer);
    
    return buffer;
}

// Pattern 8: Cleanup resources
void cleanup(WGPUDevice device, WGPUQueue queue, WGPUSwapChain swapChain,
             WGPURenderPipeline pipeline, WGPUShaderModule shader,
             WGPUBuffer buffer) {
    wgpuBufferRelease(buffer);
    wgpuRenderPipelineRelease(pipeline);
    wgpuShaderModuleRelease(shader);
    wgpuSwapChainRelease(swapChain);
    wgpuQueueRelease(queue);
    wgpuDeviceRelease(device);
}
```