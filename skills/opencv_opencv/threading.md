# Threading

### Thread safety guarantees
OpenCV's `cv::Mat` is NOT thread-safe for concurrent read/write access. Each thread should have its own copy or use proper synchronization.

```cpp
// BAD: Shared Mat access from multiple threads
cv::Mat shared = cv::imread("photo.jpg");
std::thread t1([&]() { process(shared); });
std::thread t2([&]() { process(shared); });  // Race condition!

// GOOD: Each thread gets its own copy
cv::Mat original = cv::imread("photo.jpg");
std::thread t1([&]() { 
    cv::Mat copy = original.clone();
    process(copy); 
});
std::thread t2([&]() { 
    cv::Mat copy = original.clone();
    process(copy); 
});
```

### Thread-safe VideoCapture access
```cpp
// BAD: Shared VideoCapture
cv::VideoCapture cap(0);
std::thread reader([&]() {
    cv::Mat frame;
    cap.read(frame);  // Not thread-safe!
});

// GOOD: Use mutex for synchronization
std::mutex mtx;
cv::VideoCapture cap(0);
std::thread reader([&]() {
    std::lock_guard<std::mutex> lock(mtx);
    cv::Mat frame;
    cap.read(frame);
});
```

### Thread-local processing
```cpp
// Each thread processes its own data
void processFrame(const cv::Mat& input) {
    cv::Mat result = input.clone();  // Thread-local copy
    cv::cvtColor(result, result, cv::COLOR_BGR2GRAY);
    // Process result...
}

std::vector<cv::Mat> frames = loadFrames();
std::vector<std::thread> threads;
for (auto& frame : frames) {
    threads.emplace_back(processFrame, std::ref(frame));
}
for (auto& t : threads) {
    t.join();
}
```

### Thread-safe cascade classifier
```cpp
// BAD: Shared classifier
cv::CascadeClassifier faceCascade;
faceCascade.load("haarcascade_frontalface_default.xml");
std::thread t1([&]() { 
    faceCascade.detectMultiScale(img1, faces1);  // Not thread-safe!
});

// GOOD: Each thread has its own classifier
std::thread t1([&]() {
    cv::CascadeClassifier localCascade;
    localCascade.load("haarcascade_frontalface_default.xml");
    localCascade.detectMultiScale(img1, faces1);
});
```

### Thread-safe DNN inference
```cpp
// BAD: Shared DNN network
cv::dnn::Net net = cv::dnn::readNet("model.onnx");
std::thread t1([&]() {
    net.setInput(blob1);
    cv::Mat out1 = net.forward();  // Not thread-safe!
});

// GOOD: Clone network for each thread
cv::dnn::Net baseNet = cv::dnn::readNet("model.onnx");
std::thread t1([&]() {
    cv::dnn::Net localNet = baseNet.clone();  // Thread-local copy
    localNet.setInput(blob1);
    cv::Mat out1 = localNet.forward();
});
```