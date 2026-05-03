# Lifecycle

### Construction
```cpp
// Default constructor - empty Mat
cv::Mat empty;

// Sized constructor
cv::Mat img(480, 640, CV_8UC3, cv::Scalar(0,0,0));

// From file
cv::Mat img = cv::imread("photo.jpg");

// From existing data (no copy)
uchar data[100];
cv::Mat mat(10, 10, CV_8UC1, data);

// Copy constructor (shallow)
cv::Mat copy = img;  // Shares data with img
```

### Destruction
```cpp
// Automatic destruction when out of scope
{
    cv::Mat img = cv::imread("photo.jpg");
    // img destroyed here
}

// Explicit release
cv::Mat img = cv::imread("photo.jpg");
img.release();  // Frees memory immediately

// For VideoCapture
cv::VideoCapture cap(0);
cap.release();  // Releases camera
```

### Move semantics
```cpp
// Move constructor (C++11)
cv::Mat createImage() {
    cv::Mat img(100, 100, CV_8UC3);
    return img;  // Move semantics applied automatically
}

cv::Mat result = createImage();  // No copy, data moved

// Move assignment
cv::Mat img1 = cv::imread("photo.jpg");
cv::Mat img2;
img2 = std::move(img1);  // img1 is now empty
```

### Resource management
```cpp
// Deep copy with clone()
cv::Mat original = cv::imread("photo.jpg");
cv::Mat deepCopy = original.clone();  // Independent data

// Shallow copy with copyTo()
cv::Mat shallow;
original.copyTo(shallow);  // Also deep copy

// ROI - no data copy
cv::Rect roi(50, 50, 100, 100);
cv::Mat sub = original(roi);  // Just header, shares data

// Ensure proper cleanup
class ImageProcessor {
    cv::Mat image;
public:
    ImageProcessor(const std::string& path) {
        image = cv::imread(path);
        if (image.empty()) throw std::runtime_error("Failed to load");
    }
    ~ImageProcessor() {
        image.release();  // Explicit cleanup
    }
};
```