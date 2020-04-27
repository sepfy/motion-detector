#ifndef DETECTOR_TVM_H_
#define DETECTOR_TVM_H_

#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include <string>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <dlpack/dlpack.h>
#include <tvm/runtime/module.h>
#include <tvm/runtime/registry.h>
#include <tvm/runtime/packed_func.h>

#include "capturer.h"
#include "detector.h"

class DetectorTVM : public Detector{
 public:
  DetectorTVM();
  ~DetectorTVM();
  
 private:
  int LoadModel(char *modelname);
  void Detect();
  void HWCToCHW(float *data, float *input);
  tvm::runtime::PackedFunc run_;
  tvm::runtime::PackedFunc get_output_;
  tvm::runtime::Module mod_;
  tvm::runtime::PackedFunc set_input_;
  DLTensor *x_;
  DLTensor *y_;
  
};

#endif  // DETECTOR_TVM_H_
