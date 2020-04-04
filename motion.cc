#include <iostream>
#include <vector>
#include <stdio.h>
#include "websockets.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
using namespace std;

#ifdef DEVEL
VideoCapture cap;
#else
#include "raspicam.h"
raspicam::RaspiCam Camera;
#endif


int main(int, char**) {

#ifdef DEVEL
  cap.open(0);
  if (!cap.isOpened()) {
    cerr << "ERROR! Unable to open camera\n";
    return -1;
  }
#else
  Camera.setWidth(WIDTH);
  Camera.setHeight(HEIGHT);
  Camera.setFrameRate(30);
  //Camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
  //Camera.set(CV_CAP_PROP_FRAME_WIDTH, 480);
  //Camera.set(CV_CAP_PROP_FRAME_HEIGHT, 270);
  //Camera.set(CV_CAP_PROP_FPS, 30);
  //Camera.setEncoding(raspicam::RASPICAM_ENCODING_JPEG);
  Camera.open();
#endif
  Websockets ws;
  ws.run();

  return 0;
}
