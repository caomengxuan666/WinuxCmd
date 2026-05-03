# Safety

### Red Line 1: Never access out-of-bounds pixels
**BAD:**
```cpp
cv::Mat img(100, 100, CV_8UC3);
img.at<cv::Vec3b>(150, 150) = cv::Vec3b(255,0,0);  // Out of bounds!
```

**GOOD:**
```cpp
cv::Mat img(100, 100, CV_8UC3);
int row = 50, col = 50;
if (row >= 0 && row < img.rows && col >= 0 && col < img.cols) {
    img.at<cv::Vec3b>(row, col) = cv::Vec3b(255,0,0);
}
```

### Red Line 2: Never use uninitialized Mat objects
**BAD:**
```cpp
cv::Mat img;  // Uninitialized
cv::imshow("Image", img);  // Undefined behavior
```

**GOOD:**
```cpp
cv::Mat img = cv::Mat::zeros(480, 640, CV_8UC3);  // Properly initialized
cv::imshow("Image", img);
```

### Red Line 3: Never mix different image types in operations
**BAD:**
```cpp
cv::Mat img8 = cv::imread("photo.jpg");  // CV_8UC3
cv::Mat img32(100, 100, CV_32FC3);
cv::add(img8, img32, img8);  // Type mismatch!
```

**GOOD:**
```cpp
cv::Mat img8 = cv::imread("photo.jpg");
cv::Mat img32;
img8.convertTo(img32, CV_32FC3, 1.0/255.0);  // Proper conversion
cv::add(img8, img32, img8);  // Still wrong - convert back
```

### Red Line 4: Never call DNN forward without setting input
**BAD:**
```cpp
cv::dnn::Net net = cv::dnn::readNet("model.onnx");
cv::Mat output = net.forward();  // No input set!
```

**GOOD:**
```cpp
cv::dnn::Net net = cv::dnn::readNet("model.onnx");
cv::Mat blob = cv::dnn::blobFromImage(img, 1.0/255.0);
net.setInput(blob);
cv::Mat output = net.forward();
```

### Red Line 5: Never use VideoCapture after it's been destroyed
**BAD:**
```cpp
cv::VideoCapture* cap = new cv::VideoCapture(0);
delete cap;
cv::Mat frame;
cap->read(frame);  // Use after free!
```

**GOOD:**
```cpp
cv::VideoCapture cap(0);
cv::Mat frame;
if (cap.isOpened()) {
    cap.read(frame);
}
// cap automatically destroyed when out of scope
```