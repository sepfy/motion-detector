#include <iostream>
#include <vector>
#include <stdio.h>
#include "websockets.h"
#include "detector_factory.h" 
#include "detector.h" 

using namespace cv;
using namespace std;

Capturer *g_capturer;
Detector *g_detector;

void *detection(void *data) {
  //Detector *detector = (Detector*)data;
  g_detector->Execute();
}


int main(int, char**) {

  g_capturer = new Capturer();
  g_detector = DetectorFactory::Create();
 
  char labelname[] = "labels.txt";
  g_detector->Init(labelname, g_capturer);
  char modelname[] = "shufflenet";
  g_detector->LoadModel(modelname);  

  Websockets ws;

  pthread_t detection_thread;
  pthread_create(&detection_thread, NULL, detection, NULL);

  while(true) {
    ws.Listen();
  }

  return 0;
}
