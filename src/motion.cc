#include <iostream>
#include <vector>
#include <stdio.h>
#include "websockets.h"
#include "detector_factory.h" 
#include "detector.h" 

using namespace cv;
using namespace std;

Capturer capturer;


void *detection(void *data) {
  Detector *detector = (Detector*)data;
  detector->Execute();
}


int main(int, char**) {

  DetectorFactory detector_factory;
  Detector *detector = detector_factory.Create();
 
  detector->Init("labels.txt", &capturer);
  detector->LoadModel("shufflenet");  
//  Websockets ws;

  pthread_t detection_thread;
  pthread_create(&detection_thread, NULL, detection, detector);

  while(true) {
//    ws.Listen();
  }

  return 0;
}
