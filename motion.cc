#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>
#include <stdio.h>
#include "network.h"

using namespace cv;
using namespace std;

Network network;

int main(int, char**)
{

    network.load("person.net", 1);

    VideoCapture cap("test.mp4");
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
int dilation_size = 10;
int dilation_type;
  if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
  else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
  else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
  Mat element = getStructuringElement( dilation_type,
                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                       Point( dilation_size, dilation_size ) );
      
	float *outputs, *inputs;
	inputs = new float[32*32*3];
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
        GaussianBlur(delta_frame, delta_frame, Size(7, 7), 0);

			  dilate(delta_frame, delta_frame, element);
        vector<vector<Point> > contours;
        findContours(delta_frame, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
        for(size_t i = 0; i < contours.size(); i++) {
          if(contourArea(contours[i]) > 10000) {
            Rect rect = boundingRect(contours[i]);
	    Mat roi1 = frame(rect);
	    Mat roi = frame(rect);
	    resize(roi, roi, Size(32, 32));
            roi.convertTo(roi, CV_32FC1);
            roi = (roi - 127.5)/127.5;
	    memcpy(inputs, roi.data, 32*32*3*sizeof(float));
	    outputs = network.inference(inputs);
	    if(outputs[1] > 0.9)  { 
	      //cout << outputs[0];
	      //cout << outputs[1];
              imshow("roi", roi1);
              rectangle(frame, rect, Scalar(0,0,255),2,1,0);
	    }
          }
        }

        count++;
        imshow("Live", frame);
	waitKey(3);
        //if (waitKey(5) >= 0)
        //    break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
