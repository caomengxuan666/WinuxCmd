# Pitfalls

### Pitfall 1: Not checking if image loaded successfully
**BAD:**
```cpp
cv::Mat img = cv::imread("nonexistent.jpg");
cv::imshow("Image", img);  // Crashes if img is empty
```

**GOOD:**
```cpp
cv::Mat img = cv::imread("nonexistent.jpg");
if (img.empty()) {
    std::cerr << "Failed to load image\n";
    return -1;
}
cv::imshow("Image", img);
```

### Pitfall 2: Assuming BGR color order
**BAD:**
```cpp
cv::Mat img = cv::imread("photo.jpg");
// img is in BGR, but treating as RGB
cv::Vec3b pixel = img.at<cv::Vec3b>(0,0);
int red = pixel[0];  // Actually blue channel
```

**GOOD:**
```cpp
cv::Mat img = cv::imread("photo.jpg");
cv::Mat rgb;
cv::cvtColor(img, rgb, cv::COLOR_BGR2RGB);
// Now pixel[0] is red
```

### Pitfall 3: Memory leak with VideoCapture
**BAD:**
```cpp
cv::VideoCapture cap("video.mp4");
cv::Mat frame;
while (cap.read(frame)) {
    // frame is not released properly
    process(frame);
}
```

**GOOD:**
```cpp
cv::VideoCapture cap("video.mp4");
cv::Mat frame;
while (cap.read(frame)) {
    cv::Mat processed = frame.clone();  // Explicit copy
    process(processed);
    processed.release();  // Explicit release
}
```

### Pitfall 4: Using waitKey incorrectly
**BAD:**
```cpp
cv::waitKey(0);  // Waits forever, but returns immediately on keypress
// No way to check which key was pressed
```

**GOOD:**
```cpp
int key = cv::waitKey(0) & 0xFF;
if (key == 'q') {
    std::cout << "Quit pressed\n";
}
```

### Pitfall 5: Not handling ROI boundaries
**BAD:**
```cpp
cv::Rect roi(100, 100, 500, 500);
cv::Mat sub = img(roi);  // Crashes if roi exceeds image bounds
```

**GOOD:**
```cpp
cv::Rect roi(100, 100, 500, 500);
roi &= cv::Rect(0, 0, img.cols, img.rows);  // Intersect with image bounds
if (roi.area() > 0) {
    cv::Mat sub = img(roi);
}
```

### Pitfall 6: Incorrect DNN input dimensions
**BAD:**
```cpp
cv::Mat blob = cv::dnn::blobFromImage(img, 1.0, cv::Size(224,224));
net.setInput(blob);
cv::Mat output = net.forward();  // May produce wrong results
```

**GOOD:**
```cpp
cv::Mat blob = cv::dnn::blobFromImage(img, 1.0/255.0, cv::Size(224,224), 
                                      cv::Scalar(0,0,0), true, false);
net.setInput(blob);
cv::Mat output = net.forward();
```

### Pitfall 7: Thread-unsafe cascade classifier
**BAD:**
```cpp
cv::CascadeClassifier faceCascade;
faceCascade.load("haarcascade_frontalface_default.xml");
// Called from multiple threads without synchronization
```

**GOOD:**
```cpp
cv::CascadeClassifier faceCascade;
faceCascade.load("haarcascade_frontalface_default.xml");
std::mutex mtx;
// In each thread:
{
    std::lock_guard<std::mutex> lock(mtx);
    faceCascade.detectMultiScale(img, faces);
}
```

### Pitfall 8: Not handling video codec issues
**BAD:**
```cpp
cv::VideoWriter writer("output.avi", cv::VideoWriter::fourcc('M','J','P','G'), 
                       30, cv::Size(640,480));
writer.write(frame);  // May fail silently if codec not available
```

**GOOD:**
```cpp
int codec = cv::VideoWriter::fourcc('M','J','P','G');
cv::VideoWriter writer("output.avi", codec, 30, cv::Size(640,480));
if (!writer.isOpened()) {
    std::cerr << "Failed to open video writer\n";
    return -1;
}
writer.write(frame);
```