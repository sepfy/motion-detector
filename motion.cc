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
#endif




int main(int, char**)
{
#ifdef DEVEL
  cap.open(0);
  if (!cap.isOpened()) {
    cerr << "ERROR! Unable to open camera\n";
    return -1;
  }
#endif
  Websockets ws;
  ws.run();


    return 0;
}
