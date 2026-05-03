# Overview

OpenCV (Open Source Computer Vision Library) is a comprehensive library for real-time computer vision, image processing, and machine learning. It provides over 2500 optimized algorithms for tasks including face detection, object recognition, camera calibration, 3D reconstruction, and video analysis. The library is written in C++ with bindings for Python, Java, and MATLAB.

**When to use OpenCV:**
- Building computer vision applications (face detection, object tracking)
- Image and video processing pipelines
- Camera calibration and 3D reconstruction
- Machine learning for vision tasks
- Real-time video analysis from cameras or files

**When NOT to use OpenCV:**
- Pure deep learning research (use PyTorch/TensorFlow directly)
- High-level image editing (use GIMP/Photoshop)
- Simple image I/O only (use stb_image or libpng)
- Production deployment requiring GPU optimization (consider specialized frameworks)

**Key design principles:**
- `cv::Mat` is the primary data structure for images and matrices
- Functions operate on `cv::Mat` objects with deep copy semantics by default
- Namespace `cv` contains all functions and classes
- BGR color order (not RGB) is the default for color images
- ROI (Region of Interest) operations are efficient and avoid data copying