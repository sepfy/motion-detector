#include <string.h>
#include <stdio.h>
#include <iostream>
#include <dirent.h>
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
    void Execute();
    void Init(char *labelname, char *modelname, Capturer *cap);
    string GetResult();
  private:

   int FindMaxIndex(float *array, int size);
   int GetDirIndex();
   vector<string> mLabels;
   float *mOutput;
   Capturer *mCap;
   Network network;
   string mDir = "Dumps"; 
};

