#ifndef DETECTOR_H_
#define DETECTOR_H_

#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include <string>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "capturer.h"

class Detector {
 public:
  virtual int LoadModel(char *modelname) = 0;
  virtual void Detect() = 0;
  void Execute();
  inline int Start() { detecting_ = true; };
  inline int Stop() { detecting_ = false; };
  void Init(char *labelname, Capturer *cap);
  string GetResult();

  float *input_;
  float *output_;

 private:
  int GetDirIndex();
  int FindMaxIndex(float *array, int size);
  void Preprocess(cv::Mat image);
  int ArgmaxOutput();  

  int kWidth = 224;
  int kHeight = 224;
  int kChannel = 3;
  Capturer *capturer_;
  string storage_dir_ = "Dumps";
  vector<string> labels_;
  float detecting_ = true;
};

#endif  // DETECTOR_H_
