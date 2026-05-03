# Quickstart

```cpp
// 1. Read and display an image
#include <opencv2/opencv.hpp>
int main() {
    cv::Mat img = cv::imread("photo.jpg");
    if (img.empty()) return -1;
    cv::imshow("Image", img);
    cv::waitKey(0);
    return 0;
}
```

```cpp
// 2. Capture video from camera
#include <opencv2/opencv.hpp>
int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) return -1;
    cv::Mat frame;
    while (cap.read(frame)) {
        cv::imshow("Camera", frame);
        if (cv::waitKey(30) >= 0) break;
    }
    return 0;
}
```

```cpp
// 3. Resize and convert color space
cv::Mat img = cv::imread("input.jpg");
cv::Mat resized, gray;
cv::resize(img, resized, cv::Size(640, 480));
cv::cvtColor(resized, gray, cv::COLOR_BGR2GRAY);
cv::imwrite("output.jpg", gray);
```

```cpp
// 4. Edge detection with Canny
cv::Mat img = cv::imread("input.jpg", cv::IMREAD_GRAYSCALE);
cv::Mat edges;
cv::Canny(img, edges, 50, 150);
cv::imshow("Edges", edges);
```

```cpp
// 5. Draw shapes and text
cv::Mat canvas(400, 400, CV_8UC3, cv::Scalar(255,255,255));
cv::circle(canvas, cv::Point(200,200), 100, cv::Scalar(0,0,255), 2);
cv::putText(canvas, "OpenCV", cv::Point(120,350), 
            cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0,255,0), 2);
```

```cpp
// 6. Template matching
cv::Mat img = cv::imread("scene.jpg");
cv::Mat templ = cv::imread("template.jpg");
cv::Mat result;
cv::matchTemplate(img, templ, result, cv::TM_CCOEFF_NORMED);
double minVal, maxVal;
cv::Point minLoc, maxLoc;
cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
```

```cpp
// 7. Face detection with Haar cascade
cv::CascadeClassifier faceCascade;
faceCascade.load("haarcascade_frontalface_default.xml");
cv::Mat img = cv::imread("group.jpg");
std::vector<cv::Rect> faces;
faceCascade.detectMultiScale(img, faces, 1.1, 3);
for (const auto& face : faces) {
    cv::rectangle(img, face, cv::Scalar(0,255,0), 2);
}
```

```cpp
// 8. Load and run a DNN model
cv::dnn::Net net = cv::dnn::readNet("model.onnx");
cv::Mat blob = cv::dnn::blobFromImage(img, 1.0/255.0, cv::Size(224,224));
net.setInput(blob);
cv::Mat output = net.forward();
```