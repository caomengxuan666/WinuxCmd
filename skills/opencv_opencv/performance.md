# Performance

### Memory allocation patterns
```cpp
// Pre-allocate for repeated operations
cv::Mat result(480, 640, CV_8UC3);  // Allocate once
for (int i = 0; i < 1000; i++) {
    cv::Mat frame = getFrame();
    cv::resize(frame, result, result.size());  // Reuse allocation
}
```

### Avoid unnecessary copies
```cpp
// BAD: Multiple copies
cv::Mat process(const cv::Mat& img) {
    cv::Mat temp = img.clone();  // Expensive copy
    cv::cvtColor(temp, temp, cv::COLOR_BGR2GRAY);
    return temp;
}

// GOOD: Use references and in-place operations
void process(cv::Mat& img) {
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);  // In-place
}
```

### Use ROI for efficient processing
```cpp
// BAD: Copying sub-image
cv::Mat full = cv::imread("large.jpg");
cv::Mat sub = full(cv::Rect(100,100,200,200)).clone();  // Copy data
process(sub);

// GOOD: Use ROI directly
cv::Mat full = cv::imread("large.jpg");
cv::Mat roi = full(cv::Rect(100,100,200,200));  // No copy
process(roi);  // Modifies original
```

### Optimize with OpenCV's built-in functions
```cpp
// BAD: Manual pixel iteration
for (int i = 0; i < img.rows; i++) {
    for (int j = 0; j < img.cols; j++) {
        img.at<uchar>(i,j) = 255 - img.at<uchar>(i,j);
    }
}

// GOOD: Use built-in functions
cv::bitwise_not(img, img);  // Much faster
```

### Use appropriate data types
```cpp
// BAD: Using 8-bit for floating point operations
cv::Mat img = cv::imread("photo.jpg");
img.convertTo(img, CV_32F);  // Conversion overhead

// GOOD: Load directly as needed
cv::Mat img = cv::imread("photo.jpg", cv::IMREAD_GRAYSCALE);
img.convertTo(img, CV_32F, 1.0/255.0);  // Single conversion
```