#ifndef DETECTOR_XNNC_H_
#define DETECTOR_XNNC_H_

#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include <string>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "capturer.h"
#include "detector.h"

class DetectorXNNC : public Detector {
 public:
  DetectorXNNC();
  ~DetectorXNNC();
  void Execute();
  int Start();
  int Stop();
  void Init(char *labelname, char *modelname, Capturer *cap);
  string GetResult();
  
};

#endif  // DETECTOR_XNNC_H_
