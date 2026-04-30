# Quickstart

```cpp
#include <boost/gil.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <iostream>

// Pattern 1: Load and display image dimensions
void load_and_show_dims() {
    boost::gil::rgb8_image_t img;
    boost::gil::read_image("input.jpg", img, boost::gil::jpeg_tag{});
    std::cout << "Width: " << img.width() << ", Height: " << img.height() << "\n";
}

// Pattern 2: Create a solid color image
void create_solid_image() {
    boost::gil::rgb8_image_t img(640, 480);
    auto view = boost::gil::view(img);
    boost::gil::fill_pixels(view, boost::gil::rgb8_pixel_t(255, 0, 0)); // Red
    boost::gil::write_view("output.jpg", view, boost::gil::jpeg_tag{});
}

// Pattern 3: Iterate over pixels
void iterate_pixels() {
    boost::gil::rgb8_image_t img;
    boost::gil::read_image("input.jpg", img, boost::gil::jpeg_tag{});
    auto view = boost::gil::view(img);
    for (int y = 0; y < view.height(); ++y) {
        auto row = view.row_begin(y);
        for (int x = 0; x < view.width(); ++x) {
            auto pixel = row[x];
            // Access channels: pixel[0] = red, pixel[1] = green, pixel[2] = blue
            pixel[0] = std::min(255, pixel[0] + 50); // Brighten red
        }
    }
    boost::gil::write_view("brightened.jpg", view, boost::gil::jpeg_tag{});
}

// Pattern 4: Convert between color spaces
void color_space_conversion() {
    boost::gil::rgb8_image_t rgb_img;
    boost::gil::read_image("input.jpg", rgb_img, boost::gil::jpeg_tag{});
    boost::gil::gray8_image_t gray_img(rgb_img.dimensions());
    boost::gil::copy_and_convert_pixels(
        boost::gil::view(rgb_img),
        boost::gil::view(gray_img)
    );
    boost::gil::write_view("grayscale.jpg", boost::gil::view(gray_img), boost::gil::jpeg_tag{});
}

// Pattern 5: Crop a subimage
void crop_image() {
    boost::gil::rgb8_image_t img;
    boost::gil::read_image("input.jpg", img, boost::gil::jpeg_tag{});
    auto src_view = boost::gil::view(img);
    auto sub_view = boost::gil::subimage_view(src_view, 100, 100, 200, 200);
    boost::gil::write_view("cropped.jpg", sub_view, boost::gil::jpeg_tag{});
}

// Pattern 6: Resize using nearest neighbor
void resize_image() {
    boost::gil::rgb8_image_t img;
    boost::gil::read_image("input.jpg", img, boost::gil::jpeg_tag{});
    boost::gil::rgb8_image_t resized(320, 240);
    auto src_view = boost::gil::view(img);
    auto dst_view = boost::gil::view(resized);
    boost::gil::resize_view(src_view, dst_view, boost::gil::nearest_neighbor_sampler{});
    boost::gil::write_view("resized.jpg", dst_view, boost::gil::jpeg_tag{});
}

// Pattern 7: Create a gradient
void create_gradient() {
    boost::gil::rgb8_image_t img(256, 256);
    auto view = boost::gil::view(img);
    for (int y = 0; y < view.height(); ++y) {
        auto row = view.row_begin(y);
        for (int x = 0; x < view.width(); ++x) {
            row[x] = boost::gil::rgb8_pixel_t(x, y, 128);
        }
    }
    boost::gil::write_view("gradient.jpg", view, boost::gil::jpeg_tag{});
}
```