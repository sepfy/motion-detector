#ifndef DETECTOR_H_
#define DETECTOR_H_

#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>
#include "capturer.h"

class Detector {
 public:
  virtual int LoadModel(char *modelname) = 0;
  virtual void Detect() = 0;
  void Execute();
  inline void Start() { detecting_ = true; };
  inline void Stop() { detecting_ = false; };
  void Init(char *labelname, Capturer *cap);
  std::string GetResult();

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
  std::string storage_dir_ = "Dumps";
  std::vector<std::string> labels_;
  float detecting_ = true;
};

#endif  // DETECTOR_H_
