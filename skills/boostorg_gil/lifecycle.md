# Lifecycle

```cpp
// CONSTRUCTION: Default, sized, and from file
boost::gil::rgb8_image_t default_img; // Empty, 0x0 dimensions
boost::gil::rgb8_image_t sized_img(640, 480); // Allocates pixel data
boost::gil::rgb8_image_t from_file;
boost::gil::read_image("input.jpg", from_file, boost::gil::jpeg_tag{});

// COPY CONSTRUCTION: Deep copy of pixel data
boost::gil::rgb8_image_t original(100, 100);
boost::gil::fill_pixels(boost::gil::view(original), 
    boost::gil::rgb8_pixel_t(255, 0, 0));
boost::gil::rgb8_image_t copy(original); // Deep copy
// Modifying copy doesn't affect original
boost::gil::fill_pixels(boost::gil::view(copy), 
    boost::gil::rgb8_pixel_t(0, 255, 0));

// MOVE SEMANTICS: Efficient transfer of ownership
boost::gil::rgb8_image_t create_image() {
    boost::gil::rgb8_image_t img(100, 100);
    return img; // Move or RVO
}

boost::gil::rgb8_image_t moved_to = create_image(); // Move construction
boost::gil::rgb8_image_t another;
another = std::move(moved_to); // Move assignment

// DESTRUCTION: Automatic cleanup
void process_temp() {
    boost::gil::rgb8_image_t temp(100, 100);
    // Use temp...
} // temp destroyed, memory freed

// RESOURCE MANAGEMENT: Views don't own data
void view_lifecycle() {
    boost::gil::rgb8_image_t img(100, 100);
    {
        auto view = boost::gil::view(img); // Non-owning reference
        view(0, 0) = boost::gil::rgb8_pixel_t(255, 0, 0);
    } // view goes out of scope, img still valid
    // img(0,0) still has modified pixel
}

// INTEROPERABILITY: Converting between image types
boost::gil::rgb8_image_t rgb(100, 100);
boost::gil::gray8_image_t gray(rgb.dimensions());
boost::gil::copy_and_convert_pixels(
    boost::gil::view(rgb),
    boost::gil::view(gray)
);
```