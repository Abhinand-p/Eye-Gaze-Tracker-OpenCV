#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

#define TPL_WIDTH 10
#define TPL_HEIGHT 10
#define WINDOW_WIDTH 20
#define WINDOW_HEIGHT 20
#define THRESHOLD 1.7

Mat framein, frame, frame1, frame2, tpl, tplnew, tplnew1, tpl1, tm, tm1, fm, em, em1, eye1, eye2, em2, img1, full, fruits, prev1, prev2, curr1, curr2;
Mat velocityX1, velocityY1, velocityX2, velocityY2;
Rect eye1rect, eye2rect;
CascadeClassifier faceCascade, eyeCascade;
VideoWriter writer;

int object_x0, object_y0, is_tracking = 0;
int object_x02, object_y02;
int ex, ey, eyex, eyey;
int coord[10];
int coord2[10];
int preveye[10];
int win_x0, win_y0, win_x02, win_y02;
int xc, yc, rad;
int xc2, yctwo, radtwo;
int eye1width, eye1height;
int eye2width, eye2height;

int fullx1, fully1, fullx2, fully2, fullx1p, fully1p, fullx2p, fully2p;
KalmanFilter KF;
RNG rng;
Mat state, process_noise, measurement, measurement_noise;

void trackObject();
Mat cropImage(const Mat &img, const Rect &region);
void detectEyes(Mat &img);

int main(int argc, char** argv) {
    // Load Haar cascades
    if (!faceCascade.load("/haarcascades/haarcascade_frontalface_alt.xml") ||
    !eyeCascade.load("/haarcascades/haarcascade_eye.xml")) {
        cerr << "Error loading cascades." << endl;
        return -1;
    }

    // Initialize video capture
    VideoCapture capture(0);
    if (!capture.isOpened()) {
        cerr << "Error: Could not open camera." << endl;
        return -1;
    }

    // Video writer
    int fps = 25;
    int frameW = 640;
    int frameH = 480;
    writer = VideoWriter("out6.avi", VideoWriter::fourcc('P', 'I', 'M', '1'), fps, Size(frameW, frameH), true);
    if (!writer.isOpened()) {
        cerr << "Error: Could not open video writer." << endl;
        return -1;
    }

    namedWindow("frame..", WINDOW_AUTOSIZE);

    while (true) {
        if (!capture.read(frame)) {
            cerr << "Error: Could not read frame." << endl;
            break;
        }

        detectEyes(frame);

        if (is_tracking) {
            trackObject();
        }

        imshow("frame..", frame);

        // Exit if 'q' is pressed
        if (waitKey(30) == 'q') {
            break;
        }
    }

    capture.release();
    writer.release();
    destroyAllWindows();
    return 0;
}

Mat cropImage(const Mat &img, const Rect &region) {
    if (region.x < 0 || region.y < 0 || region.x + region.width > img.cols || region.y + region.height > img.rows) {
        cerr << "ERROR in cropImage(): invalid dimensions." << endl;
        exit(1);
    }

    return img(region).clone();
}

void detectEyes(Mat &img) {
    vector<Rect> faces;
    faceCascade.detectMultiScale(img, faces, 1.1, 3, 0, Size(40, 40));

    if (faces.empty()) return;

    Rect face = faces[0];
    Mat faceROI = img(face);

    vector<Rect> eyes;
    eyeCascade.detectMultiScale(faceROI, eyes, 1.15, 3, 0, Size(25, 15));

    if (eyes.size() < 2) return;

    // Crop the eyes
    eye1rect = eyes[0];
    eye2rect = eyes[1];
    eye1 = cropImage(faceROI, eye1rect);
    eye2 = cropImage(faceROI, eye2rect);

    // Display detected eyes
    imshow("Eye 1", eye1);
    imshow("Eye 2", eye2);

    // Convert to grayscale for further processing
    Mat eye1Gray, eye2Gray;
    cvtColor(eye1, eye1Gray, COLOR_BGR2GRAY);
    cvtColor(eye2, eye2Gray, COLOR_BGR2GRAY);

    // Apply HoughCircles to detect the pupils
    vector<Vec3f> circles1, circles2;
    HoughCircles(eye1Gray, circles1, HOUGH_GRADIENT, 1, eye1Gray.rows/8, 200, 20, 5, 15);
    HoughCircles(eye2Gray, circles2, HOUGH_GRADIENT, 1, eye2Gray.rows/8, 200, 20, 5, 15);

    // If circles are detected, start tracking
    if (!circles1.empty() && !circles2.empty()) {
        Point center1(cvRound(circles1[0][0]), cvRound(circles1[0][1]));
        int radius1 = cvRound(circles1[0][2]);
        Point center2(cvRound(circles2[0][0]), cvRound(circles2[0][1]));
        int radius2 = cvRound(circles2[0][2]);

        // Draw the circles
        circle(eye1, center1, radius1, Scalar(255, 0, 0), 2);
        circle(eye2, center2, radius2, Scalar(255, 0, 0), 2);

        // Set tracking flag and object centers
        is_tracking = 1;
        fullx1 = center1.x + face.x + eye1rect.x;
        fully1 = center1.y + face.y + eye1rect.y;
        fullx2 = center2.x + face.x + eye2rect.x;
        fully2 = center2.y + face.y + eye2rect.y;
    }
}

void trackObject() {
    // Setup search windows
    Rect searchWindow1(fullx1 - WINDOW_WIDTH/2, fully1 - WINDOW_HEIGHT/2, WINDOW_WIDTH, WINDOW_HEIGHT);
    Rect searchWindow2(fullx2 - WINDOW_WIDTH/2, fully2 - WINDOW_HEIGHT/2, WINDOW_WIDTH, WINDOW_HEIGHT);

    if (searchWindow1.x < 0 || searchWindow1.y < 0 || searchWindow1.x + searchWindow1.width > frame.cols || searchWindow1.y + searchWindow1.height > frame.rows ||
        searchWindow2.x < 0 || searchWindow2.y < 0 || searchWindow2.x + searchWindow2.width > frame.cols || searchWindow2.y + searchWindow2.height > frame.rows) {
        cerr << "ERROR: Search window out of frame bounds." << endl;
        is_tracking = 0;
        return;
    }

    Mat result1, result2;
    matchTemplate(frame(searchWindow1), eye1, result1, TM_CCOEFF);
    matchTemplate(frame(searchWindow2), eye2, result2, TM_CCOEFF);

    double minVal1, maxVal1, minVal2, maxVal2;
    Point minLoc1, maxLoc1, minLoc2, maxLoc2;
    minMaxLoc(result1, &minVal1, &maxVal1, &minLoc1, &maxLoc1);
    minMaxLoc(result2, &minVal2, &maxVal2, &minLoc2, &maxLoc2);

    // Update object positions
    fullx1 = searchWindow1.x + maxLoc1.x;
    fully1 = searchWindow1.y + maxLoc1.y;
    fullx2 = searchWindow2.x + maxLoc2.x;
    fully2 = searchWindow2.y + maxLoc2.y;

    // Draw tracking rectangles
    rectangle(frame, Rect(fullx1 - TPL_WIDTH/2, fully1 - TPL_HEIGHT/2, TPL_WIDTH, TPL_HEIGHT), Scalar(0, 255, 0), 2);
    rectangle(frame, Rect(fullx2 - TPL_WIDTH/2, fully2 - TPL_HEIGHT/2, TPL_WIDTH, TPL_HEIGHT), Scalar(0, 255, 0), 2);

    // Display the result
    imshow("Tracking", frame);
    writer.write(frame);
}