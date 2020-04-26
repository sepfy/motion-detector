#ifndef CAPTURER_H_
#define CAPTURER_H_

#include <stdio.h>
#include <string.h>

#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "cpp-base64/base64.h"

#ifndef DEVEL
#include "raspicam.h"
#endif 

using namespace std;
using namespace cv;

class Capturer { 
  public:
    Capturer();
    ~Capturer();

   bool imageReady = false;

   void Grab(void);
   void Dump(char *filename);
   string GetBase64Image();
   Mat GetFrame();
   void Start();
   void Stop();

  private:
    bool bRunning = false;
    int width = 224;
    int height = 224;
    int channel = 3;
    Mat mFrame;
#ifdef DEVEL
    VideoCapture camera;
#else
    raspicam::RaspiCam camera;
    unsigned char *data;
#endif

};

#endif  // CAPTURER_H_
