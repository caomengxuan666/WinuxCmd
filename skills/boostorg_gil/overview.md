# Overview

Boost.GIL (Generic Image Library) provides a generic, extensible framework for image processing in C++. It separates image representation from algorithms, allowing algorithms to work with any image type (color space, channel depth, memory layout) without modification. The library supports common operations like pixel access, color conversion, resizing, and I/O through extension modules.

**When to use:**
- You need to process images in C++ with type safety and performance
- You want algorithms that work across different image formats and color spaces
- You need to integrate image processing into existing Boost-based projects
- You require compile-time polymorphism for image types

**When NOT to use:**
- For simple image loading/saving without processing (use stb_image or FreeImage)
- When you need GPU acceleration (consider OpenCV with CUDA)
- For real-time video processing (GIL is designed for still images)
- When binary size is critical (GIL templates can increase compilation time)

**Key design concepts:**
- **Image**: owns pixel data (e.g., `rgb8_image_t`)
- **View**: non-owning reference to pixel data (e.g., `rgb8_view_t`)
- **Pixel**: single element with channels (e.g., `rgb8_pixel_t`)
- **Channel**: individual color component (e.g., `uint8_t` for 8-bit)
- **Locator**: random-access iterator over pixels
- **Algorithm**: templated functions operating on views (e.g., `fill_pixels`, `copy_pixels`)