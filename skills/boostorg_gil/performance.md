# Performance

```cpp
// PERFORMANCE CHARACTERISTICS
// GIL uses templates extensively - most operations are inlined
// Pixel access is O(1) through locators
// Memory allocation happens once during construction

// ALLOCATION PATTERNS
// BAD: Frequent reallocation
boost::gil::rgb8_image_t img;
for (int i = 0; i < 100; ++i) {
    img = boost::gil::rgb8_image_t(640, 480); // Reallocates each iteration
    // Process img...
}

// GOOD: Reuse existing allocation
boost::gil::rgb8_image_t img(640, 480);
auto view = boost::gil::view(img);
for (int i = 0; i < 100; ++i) {
    // Reuse same memory, just overwrite pixels
    boost::gil::fill_pixels(view, boost::gil::rgb8_pixel_t(i, i, i));
}

// OPTIMIZATION TIP 1: Use interleaved views for cache efficiency
// BAD: Planar access pattern
auto view = boost::gil::view(img);
for (int c = 0; c < 3; ++c) {
    for (int y = 0; y < view.height(); ++y) {
        auto row = view.row_begin(y);
        for (int x = 0; x < view.width(); ++x) {
            // Accessing channel c for each pixel - poor cache locality
            process_channel(row[x][c]);
        }
    }
}

// GOOD: Per-pixel access
for (int y = 0; y < view.height(); ++y) {
    auto row = view.row_begin(y);
    for (int x = 0; x < view.width(); ++x) {
        auto& pixel = row[x];
        process_pixel(pixel[0], pixel[1], pixel[2]); // Sequential access
    }
}

// OPTIMIZATION TIP 2: Use raw iterators for tight loops
// BAD: Repeated bounds checking
for (int y = 0; y < view.height(); ++y) {
    for (int x = 0; x < view.width(); ++x) {
        view(x, y) = pixel; // Slower due to bounds checking
    }
}

// GOOD: Use row iterators
for (int y = 0; y < view.height(); ++y) {
    auto row_start = view.row_begin(y);
    auto row_end = view.row_end(y);
    for (auto it = row_start; it != row_end; ++it) {
        *it = pixel; // No bounds checking
    }
}

// OPTIMIZATION TIP 3: Prefer views over images for algorithms
// BAD: Creating temporary images
boost::gil::rgb8_image_t process(const boost::gil::rgb8_image_t& img) {
    boost::gil::rgb8_image_t result(img.dimensions());
    // Process...
    return result;
}

// GOOD: Work with views
void process_view(const boost::gil::rgb8_view_t& src, 
                  const boost::gil::rgb8_view_t& dst) {
    // Process directly on views
}
```