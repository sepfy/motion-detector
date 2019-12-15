#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>
#include <stdio.h>
using namespace cv;
using namespace std;
int main(int, char**)
{
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    //--- GRAB AND WRITE LOOP
    cout << "Start grabbing" << endl
        << "Press any key to terminate" << endl;
    int count = 0;
    Mat prev_frame;
    cap.read(prev_frame);
        cvtColor(prev_frame, prev_frame, CV_BGR2GRAY);
    Mat delta_frame;
    Mat current_frame;
    vector<Vec4i> hierarchy;


int dilation_elem = 2;
int dilation_size = 20;
int dilation_type;
  if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
  else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
  else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
  Mat element = getStructuringElement( dilation_type,
                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                       Point( dilation_size, dilation_size ) );

    for (;;)
    {
        // wait for a new frame from camera and store it into 'frame'
    Mat frame;
        cap.read(frame);
        // check if we succeeded
        if (frame.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        
        Mat blur, gray;
        cvtColor(frame, gray, CV_BGR2GRAY);
        absdiff(prev_frame, gray, delta_frame);
        if(count % 3 == 0) {
          prev_frame = gray;
        }

        threshold(delta_frame, delta_frame, 20, 255.0, CV_THRESH_BINARY);
        imshow("Live", delta_frame);
        GaussianBlur(delta_frame, delta_frame, Size(3, 3), 0);

			  dilate(delta_frame, delta_frame, element);
        vector<vector<Point> > contours;
        findContours(delta_frame, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
        for(size_t i = 0; i < contours.size(); i++) {
          if(contourArea(contours[i]) > 10000) {
            Rect rect = boundingRect(contours[i]);
            rectangle(frame, rect, Scalar(0,0,255),2,1,0);
          }
        }

        count++;
        if (waitKey(5) >= 0)
            break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
