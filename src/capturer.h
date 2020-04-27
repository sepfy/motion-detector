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

class Capturer { 
 public:
  Capturer();
  ~Capturer();

 void Grab(void);
 void Dump(char *filename);
 std::string GetBase64Image();
 cv::Mat GetFrame();

 private:
  int width = 224;
  int height = 224;
  int channel = 3;
  cv::Mat frame_;
#ifdef DEVEL
  cv::VideoCapture camera_;
#else
  raspicam::RaspiCam camera_;
  unsigned char *data;
#endif

};

#endif  // CAPTURER_H_
