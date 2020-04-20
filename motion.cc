#include <iostream>
#include <vector>
#include <stdio.h>
#include "websockets.h"
#include "detector.h" 

using namespace cv;
using namespace std;

Capturer capturer;
Detector detector;


void *detector_thread(void *data) {
  detector.Execute();
}


int main(int, char**) {

  detector.Init("labels.txt", "resnet.xnor.net", &capturer);
  Websockets ws;

  pthread_t tdetector;
  pthread_create(&tdetector, NULL, detector_thread, NULL);

  while(true) {
    ws.Listen();
  }

  return 0;
}
