# Best Practices

### Use RAII for resource management
```cpp
class VideoProcessor {
    cv::VideoCapture cap;
    cv::VideoWriter writer;
public:
    VideoProcessor(const std::string& input, const std::string& output) {
        cap.open(input);
        if (!cap.isOpened()) throw std::runtime_error("Cannot open input");
        writer.open(output, cv::VideoWriter::fourcc('M','J','P','G'), 
                    30, cv::Size(640,480));
    }
    ~VideoProcessor() {
        cap.release();
        writer.release();
    }
};
```

### Prefer clone() for explicit deep copies
```cpp
cv::Mat original = cv::imread("photo.jpg");
cv::Mat shallow = original;  // Shallow copy - shares data
cv::Mat deep = original.clone();  // Deep copy - independent data
original.setTo(cv::Scalar(0,0,0));  // shallow is also modified, deep is not
```

### Use ROI for efficient sub-image operations
```cpp
cv::Mat img = cv::imread("large.jpg");
cv::Rect roi(100, 100, 200, 200);
cv::Mat sub = img(roi);  // No data copy - just header
cv::cvtColor(sub, sub, cv::COLOR_BGR2GRAY);  // Modifies original!
```

### Handle exceptions properly
```cpp
try {
    cv::Mat img = cv::imread("photo.jpg");
    if (img.empty()) {
        throw std::runtime_error("Failed to load image");
    }
    // Process image
} catch (const cv::Exception& e) {
    std::cerr << "OpenCV error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}
```

### Use cv::Ptr for smart pointer management
```cpp
cv::Ptr<cv::Feature2D> detector = cv::SIFT::create();
std::vector<cv::KeyPoint> keypoints;
cv::Mat descriptors;
detector->detectAndCompute(img, cv::noArray(), keypoints, descriptors);
// Automatically deleted when out of scope
```