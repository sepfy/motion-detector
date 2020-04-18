#include <string.h>
#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "capturer.h"
#include "network.h"
#ifndef DEVEL
#include "raspicam.h"
#endif

using namespace cv;
using namespace std;

class Detector {
  public:
    Detector();
    ~Detector();
    void execute();
    void init(char *labelname, char *modelname, Capturer *cap);
    string get_json_result();
  private:
   vector<string> mLabels;
   float *mOutput;
   Capturer *mCap;
   int find_max_index(float *array, int size);
   Network network;
};

