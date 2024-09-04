# Eye-Gaze-Tracker-OpenCV

## Overview
This project is a real-time gaze tracking system implemented in C++ using OpenCV. It detects and tracks eye movements to determine the direction of gaze, which can be used in various applications such as human-computer interaction and assistive technologies.

## Features
- Eye and face detection using Haar cascades.
- Real-time video processing with OpenCV.
- Gaze estimation using optical flow.

## Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/Abhinand-p/Eye-Gaze-Tracker-OpenCV.git
   ```
2. Install OpenCV:
   Follow the instructions on the [OpenCV installation page](https://opencv.org/) to install OpenCV for C++.

## Usage
Compile and run the `code.cpp` file:
```bash
g++ code.cpp -o gaze_tracking `pkg-config --cflags --libs opencv4`
./gaze_tracking
```

## Dependencies
- OpenCV 4.x
