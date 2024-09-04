
# OpenCV Gaze Tracking Update

This repository contains both the original and updated versions of an eye gaze tracking program. The original code, written 8 years ago, used the now-deprecated C API of OpenCV. The updated version leverages the modern C++ API of OpenCV, improving compatibility with current systems and enhancing maintainability and performance.

## Repository Structure

- **`code.cpp/`**: Contains the original code using the OpenCV C API.
- **`new_code/`**: Contains the updated code (`main.cpp`) using the modern OpenCV C++ API.
- **`haarcascades/`**: Directory to store required Haar Cascade XML files for face and eye detection.
- **`README.md`**: This file, providing setup and usage instructions.

## Changes and Improvements in the New Code

- **Modernized OpenCV API**: Replaced deprecated C API functions with the modern C++ equivalents (`cv::Mat` instead of `IplImage*`, `CascadeClassifier` instead of `CvHaarClassifierCascade`, etc.).
- **Improved Memory Management**: The updated code uses `cv::Mat`, which automatically handles memory allocation and deallocation.
- **Better Error Handling**: Enhanced error checking for file loading, camera access, and window creation to prevent crashes and undefined behavior.
- **Optimized Tracking**: Retained the original logic for template matching and tracking while ensuring compatibility with the latest OpenCV standards.
- **Simplified File I/O**: Updated code utilizes modern C++ file handling for reading and writing tracking data.

## Prerequisites

To run either version of this program, you'll need:

- OpenCV (version 4.x or newer) installed on your system.
- A C++ compiler compatible with C++11 or newer.
- A working webcam connected to your system.

## Setup Instructions

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/yourusername/Eye-Gaze-Tracker-OpenCV.git
   cd Eye-Gaze-Tracker-OpenCV
   ```

2. **Install OpenCV**:

   Ensure that OpenCV is installed on your system. You can install it via package managers like `apt` on Ubuntu, `brew` on macOS, or download it from the [OpenCV website](https://opencv.org/).

   - For macOS:
     ```bash
     brew install opencv
     ```

   - For Ubuntu:
     ```bash
     sudo apt update
     sudo apt install libopencv-dev
     ```

3. **Download Haar Cascade XML Files**:

   The program requires Haar Cascade XML files for face and eye detection. Download the following files:

   - [haarcascade_frontalface_alt.xml](https://github.com/opencv/opencv/blob/master/data/haarcascades/haarcascade_frontalface_alt.xml)
   - [haarcascade_eye.xml](https://github.com/opencv/opencv/blob/master/data/haarcascades/haarcascade_eye.xml)

   Place these files in the `haarcascades/` directory within the project, or specify their paths in the code.

4. **Build and Run the New Code**:

   If using CLion or another IDE that uses CMake, follow these steps:

   - Open the project in CLion or your preferred C++ IDE.
   - Make sure the `CMakeLists.txt` file is set up correctly in the `new_code/` directory:

   ```cmake
   cmake_minimum_required(VERSION 3.16)
   
   project(Eye_Gaze_Tracker_OpenCV)

   set(CMAKE_CXX_STANDARD 17)

   find_package(OpenCV REQUIRED)
   
   include_directories(${OpenCV_INCLUDE_DIRS})

   add_executable(${PROJECT_NAME} new_code/main.cpp)
   
   target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
   ```

   - Build the project using the IDE's build tools.

   If using the command line:

   ```bash
   cd new_code
   mkdir build
   cd build
   cmake ..
   make
   ```

   **Run the New Code**:

   ```bash
   ./Eye_Gaze_Tracker_OpenCV
   ```

   Ensure your webcam is connected and properly set up.

5. **Run the Old Code**:

   To run the old code (`code.cpp`), you may need to compile it with compatibility options. The original code is written using the deprecated C API, so ensure you have OpenCV installed with backward compatibility enabled.

   - **Compile the Old Code**:

   ```bash
   g++ old_code/code.cpp -o Eye_Gaze_Tracker_OpenCV_old `pkg-config --cflags --libs opencv`
   ```

   - **Run the Old Code**:

   ```bash
   ./Eye_Gaze_Tracker_OpenCV_old
   ```

## Execution Steps for Both Versions

### New Code (Modern C++ API)

1. **Start the Program**:
   - The program initializes the webcam and starts capturing video frames.

2. **Face and Eye Detection**:
   - The program uses Haar Cascade Classifiers to detect faces and eyes within the captured frames.
   - Detected faces and eyes are highlighted with rectangles in the output video.

3. **Pupil Detection**:
   - After detecting eyes, the program applies Hough Circle Transform to detect pupils within the eyes.

4. **Gaze Tracking**:
   - If pupils are successfully detected, the program initiates tracking of the pupils' positions across subsequent frames using template matching.

5. **Visual Output**:
   - Multiple OpenCV windows display the tracking results, including the original frame, detected eyes, and tracking rectangles.

6. **Exit the Program**:
   - Press 'q' to exit the program.

### Old Code (C API)

1. **Start the Program**:
   - The program initializes the webcam and begins capturing video frames.

2. **Face and Eye Detection**:
   - Uses Haar Cascades loaded from XML files to detect faces and eyes in the frames.

3. **Gaze Tracking**:
   - Performs pupil detection and tracking using template matching and optical flow.

4. **Visual Output**:
   - Opens multiple windows for each step of the detection and tracking process.

5. **Exit the Program**:
   - Press 'q' or terminate the console to stop the program.

## Additional Notes

- The video with tracking results is saved as `out6.avi` in the root directory of the project.
- Adjust parameters for detection and tracking if needed to improve performance under different lighting conditions or camera setups.