#include <iostream>
#include <vector>
#include <stdio.h>
#include "websockets.h"

using namespace std;

#ifdef DEVEL
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
VideoCapture cap;
#else
#include "raspicam_still.h"
raspicam::RaspiCam_Still Camera;
#endif


int main(int, char**) {

#ifdef DEVEL
  cap.open(0);
  if (!cap.isOpened()) {
    cerr << "ERROR! Unable to open camera\n";
    return -1;
  }
#else
  Camera.setWidth(480);
  Camera.setHeight(270);
  Camera.setEncoding(raspicam::RASPICAM_ENCODING_JPEG);
  Camera.open();
#endif
  Websockets ws;
  ws.run();

  return 0;
}
