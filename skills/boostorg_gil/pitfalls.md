# Pitfalls

```cpp
// PITFALL 1: Using uninitialized image dimensions
// BAD: Creating image without specifying dimensions
boost::gil::rgb8_image_t img; // Default constructed - no dimensions!
auto view = boost::gil::view(img); // Undefined behavior - view has no data

// GOOD: Always specify dimensions
boost::gil::rgb8_image_t img(640, 480);
auto view = boost::gil::view(img); // Safe

// PITFALL 2: Mismatched pixel types in copy operations
// BAD: Copying between incompatible pixel types
boost::gil::rgb8_image_t rgb_img(100, 100);
boost::gil::gray8_image_t gray_img(100, 100);
// boost::gil::copy_pixels(view(rgb_img), view(gray_img)); // Compile error!

// GOOD: Use copy_and_convert_pixels for type conversion
boost::gil::copy_and_convert_pixels(
    boost::gil::view(rgb_img),
    boost::gil::view(gray_img)
);

// PITFALL 3: Out-of-bounds pixel access
// BAD: Accessing pixels beyond image bounds
auto view = boost::gil::view(img);
// view(1000, 1000) = pixel; // Undefined behavior if image is smaller

// GOOD: Check bounds before access
if (x < view.width() && y < view.height()) {
    view(x, y) = pixel;
}

// PITFALL 4: Forgetting to include I/O extension headers
// BAD: Missing JPEG header
#include <boost/gil.hpp>
// boost::gil::read_image("img.jpg", img, boost::gil::jpeg_tag{}); // Link error!

// GOOD: Include proper extension
#include <boost/gil/extension/io/jpeg.hpp>
boost::gil::read_image("img.jpg", img, boost::gil::jpeg_tag{});

// PITFALL 5: Using view after image destruction
// BAD: Dangling view
auto get_view() {
    boost::gil::rgb8_image_t img(100, 100);
    return boost::gil::view(img); // Returns view to destroyed image!
}

// GOOD: Return image or copy data
boost::gil::rgb8_image_t get_image() {
    boost::gil::rgb8_image_t img(100, 100);
    return img; // Safe - moves the image
}

// PITFALL 6: Incorrect channel ordering assumptions
// BAD: Assuming RGB order for all images
auto pixel = view(0, 0);
// pixel[0] might be Blue in some layouts!

// GOOD: Use named channel accessors
boost::gil::get_color(pixel, boost::gil::red_t());
boost::gil::get_color(pixel, boost::gil::green_t());
boost::gil::get_color(pixel, boost::gil::blue_t());

// PITFALL 7: Not handling image loading failures
// BAD: No error checking
boost::gil::rgb8_image_t img;
boost::gil::read_image("nonexistent.jpg", img, boost::gil::jpeg_tag{}); // Throws!

// GOOD: Use try-catch
try {
    boost::gil::read_image("nonexistent.jpg", img, boost::gil::jpeg_tag{});
} catch (const std::exception& e) {
    std::cerr << "Failed to load image: " << e.what() << "\n";
}
```