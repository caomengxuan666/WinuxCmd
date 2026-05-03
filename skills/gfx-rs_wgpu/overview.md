# Overview

```cpp
// wgpu is a safe and portable graphics API that implements the WebGPU standard natively
// It provides modern GPU programming capabilities across Vulkan, Metal, D3D12, and OpenGL

#include <webgpu/webgpu.h>

// When to use wgpu:
// - Cross-platform GPU programming (Windows, macOS, Linux, Android, iOS)
// - Need for modern GPU features (compute shaders, ray tracing)
// - Safe memory management with automatic resource tracking
// - When WebGPU compatibility is desired for future web deployment

// When NOT to use wgpu:
// - Maximum performance on a single platform (use native APIs)
// - Legacy GPU support (OpenGL 3.3+ only)
// - Very simple 2D rendering (consider Skia or Cairo)

// Key design principles:
// 1. Explicit resource management with reference counting
// 2. All GPU operations go through command encoders
// 3. Pipeline state objects are immutable after creation
// 4. Buffer and texture usage flags must be declared upfront

// Example: Checking device features
void checkFeatures(WGPUDevice device) {
    size_t count = wgpuDeviceEnumerateFeatures(device, nullptr);
    std::vector<WGPUFeatureName> features(count);
    wgpuDeviceEnumerateFeatures(device, features.data());
    
    bool hasCompute = false;
    for (auto f : features) {
        if (f == WGPUFeatureName_ShaderF16) {
            hasCompute = true;
        }
    }
}
```