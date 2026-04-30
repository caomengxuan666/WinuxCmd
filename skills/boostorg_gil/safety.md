# Safety

```cpp
// RED LINE 1: NEVER access pixels through a default-constructed view
// BAD: Undefined behavior
boost::gil::rgb8_view_t empty_view;
// empty_view(0, 0) = pixel; // CRASH!

// GOOD: Always check view is valid
if (!empty_view.dimensions().x || !empty_view.dimensions().y) {
    // Handle empty view
}

// RED LINE 2: NEVER use view after its parent image is destroyed
// BAD: Dangling reference
boost::gil::rgb8_view_t dangerous_view;
{
    boost::gil::rgb8_image_t img(100, 100);
    dangerous_view = boost::gil::view(img);
} // img destroyed, dangerous_view is dangling
// dangerous_view(0, 0) = pixel; // CRASH!

// GOOD: Keep image alive while using view
boost::gil::rgb8_image_t img(100, 100);
auto safe_view = boost::gil::view(img);
safe_view(0, 0) = boost::gil::rgb8_pixel_t(255, 0, 0); // Safe

// RED LINE 3: NEVER mix different channel types without conversion
// BAD: Direct assignment between different channel depths
boost::gil::rgb8_image_t img8(100, 100);
boost::gil::rgb16_image_t img16(100, 100);
// boost::gil::copy_pixels(view(img8), view(img16)); // Compile error or UB

// GOOD: Use proper conversion
boost::gil::copy_and_convert_pixels(
    boost::gil::view(img8),
    boost::gil::view(img16)
);

// RED LINE 4: NEVER use subimage_view with invalid coordinates
// BAD: Out-of-bounds subimage
auto view = boost::gil::view(img);
// auto sub = boost::gil::subimage_view(view, -1, -1, 1000, 1000); // UB!

// GOOD: Validate coordinates
int x = 0, y = 0, w = 100, h = 100;
if (x >= 0 && y >= 0 && x + w <= view.width() && y + h <= view.height()) {
    auto sub = boost::gil::subimage_view(view, x, y, w, h);
}

// RED LINE 5: NEVER assume image data is contiguous in memory
// BAD: Treating view as flat array
auto view = boost::gil::view(img);
// uint8_t* data = reinterpret_cast<uint8_t*>(&view(0, 0)); // Wrong!
// for (int i = 0; i < view.width() * view.height() * 3; ++i) { ... } // UB

// GOOD: Use proper iterators
for (int y = 0; y < view.height(); ++y) {
    auto row = view.row_begin(y);
    for (int x = 0; x < view.width(); ++x) {
        // Process row[x] safely
    }
}
```