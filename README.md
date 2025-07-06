# OpenCV Camera Viewer with Face Detection

A GUI application to open and display a camera feed using C++ with OpenCV and Qt. The application also includes face detection functionality.

## Prerequisites

- CMake (3.10 or higher)
- C++ compiler with C++11 support
- OpenCV library
- Qt5 library

## Setting Up Face Detection

1. Download the Haar Cascade face detection model:
   - You can download it from OpenCV's GitHub repository: https://github.com/opencv/opencv/blob/master/data/haarcascades/haarcascade_frontalface_default.xml
   - Save this file in the same directory as the executable (or specify the correct path in the code)

## Building the Application

1. Make sure you have the prerequisites installed
2. Create a build directory and navigate to it:

```bash
mkdir build
cd build
```

3. Run CMake and build:

```bash
cmake ..
make
```

## Running the Application

From the build directory, run:

```bash
./camera_app
```

## Usage

- Click "Start Camera" to begin capturing from the default camera
- Click "Stop Camera" to stop the video capture
- Click "Start Face Detection" to enable real-time face detection
- Click "Stop Face Detection" to disable face detection

## Troubleshooting

If you encounter an error opening the camera, make sure:
- Your webcam is properly connected
- No other application is using the camera
- You have appropriate permissions to access the camera

If face detection is not working:
- Make sure the haarcascade_frontalface_default.xml file is correctly located in the application directory
- Check the console for any error messages related to loading the face detection model # simple-face-detection-with-Qt
