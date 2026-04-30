# Threading

```cpp
// THREAD SAFETY GUARANTEES
// GIL views are NOT thread-safe for concurrent writes
// GIL images are NOT thread-safe for concurrent modifications
// Reading from const views is safe across threads

// BAD: Concurrent writes to same view
boost::gil::rgb8_image_t img(1000, 1000);
auto view = boost::gil::view(img);

std::vector<std::thread> threads;
for (int i = 0; i < 4; ++i) {
    threads.emplace_back([&view, i]() {
        // Multiple threads writing to same view - DATA RACE!
        for (int y = i * 250; y < (i + 1) * 250; ++y) {
            auto row = view.row_begin(y);
            for (int x = 0; x < view.width(); ++x) {
                row[x] = boost::gil::rgb8_pixel_t(255, 0, 0);
            }
        }
    });
}

// GOOD: Partition image into non-overlapping regions
boost::gil::rgb8_image_t img(1000, 1000);
auto view = boost::gil::view(img);

void process_region(const boost::gil::rgb8_view_t& region) {
    for (int y = 0; y < region.height(); ++y) {
        auto row = region.row_begin(y);
        for (int x = 0; x < region.width(); ++x) {
            row[x] = boost::gil::rgb8_pixel_t(255, 0, 0);
        }
    }
}

std::vector<std::thread> threads;
int strip_height = view.height() / 4;
for (int i = 0; i < 4; ++i) {
    int y_start = i * strip_height;
    auto sub_view = boost::gil::subimage_view(
        view, 0, y_start, view.width(), strip_height);
    threads.emplace_back(process_region, sub_view); // Safe - non-overlapping
}

// CONCURRENT ACCESS PATTERN: Read-only access is safe
boost::gil::rgb8_image_t img(1000, 1000);
const auto const_view = boost::gil::const_view(img);

std::vector<std::thread> readers;
for (int i = 0; i < 4; ++i) {
    readers.emplace_back([&const_view, i]() {
        // Multiple threads reading same const view - SAFE
        int sum = 0;
        for (int y = 0; y < const_view.height(); ++y) {
            auto row = const_view.row_begin(y);
            for (int x = 0; x < const_view.width(); ++x) {
                sum += row[x][0];
            }
        }
    });
}

// SYNCHRONIZATION PATTERN: Use mutex for shared mutable access
std::mutex img_mutex;
boost::gil::rgb8_image_t shared_img(100, 100);

void safe_modify() {
    std::lock_guard<std::mutex> lock(img_mutex);
    auto view = boost::gil::view(shared_img);
    view(0, 0) = boost::gil::rgb8_pixel_t(255, 0, 0);
}
```