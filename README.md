# Project Name: OpenCv-Gaze-tracking

## Code Description:
This code implements a gaze tracking system using OpenCV. It primarily involves tracking the user's gaze using image processing techniques.

## Dependencies:
The code depends on the OpenCV library, which is used for image processing and computer vision tasks.

## Main Components:
The code defines several global variables, including image objects, classifiers, and storage structures for processing images and tracking gaze.

## Functionality:
The code uses Haar cascade classifiers to detect faces and eyes in video frames.
It then performs gaze tracking by identifying and tracking the eyes.
The code computes optical flow to estimate eye movement.
It utilizes a threshold value to determine when an object (e.g., the eyes) is found.
When an object is found, it draws a box around the detected object.
The code also captures video frames and saves them to an output video file.

## Usage:
The code initializes a camera to capture video frames.
It loads Haar cascade classifiers for face and eye detection.
It captures and processes frames in a loop.
When it detects an object (e.g., an eye), it tracks its movement and draws a box around it.
The processed video frames are saved to an output video file.

## Key Variables:
framein, frame, tpl, tplnew, tplnew1, tpl1, tm, tm1: Image objects for storing frames and templates.
cascade_f and cascade_e: Haar cascade classifiers for face and eye detection.
storage, storage1, storage2: Memory storage for processing results.
Various other variables for storing object coordinates, window sizes, and video capture.

## How to Use:
The code is set to run a loop to capture and process video frames.
You can start the program, and it will continuously track the gaze.
