#include "detector.h"


Detector::Detector() {

}

Detector::~Detector() {

}


void Detector::init(char *labelname, char *modelname, Capturer *cap) {

  ifstream file(labelname);
  string s;
  while (std::getline(file, s)) {
    mLabels.push_back(s); 
  }
  file.close();

  mOutput = new float[mLabels.size()];

  network.load(modelname, 1);

  mCap = cap;
}

int Detector::find_max_index(float *array, int size) {

  float max = -10;
  int max_idx = -1;
  for(int i = 0; i < size; i++) {
    if(array[i] > max) {
      max = array[i];
      max_idx = i;
    }
  }
  array[max_idx] = -1;
  return max_idx;
}


string Detector::get_json_result() {

  string tmp;
  int n = mLabels.size();
  float *array = new float[n];
  float *output = new float[n];
  memcpy(array, mOutput, sizeof(float)*n);
  memcpy(output, mOutput, sizeof(float)*n);

  tmp = "{\"detect\":{";
  for(int i = 0; i < n; i++) {
    int index = find_max_index(array, n);
    tmp += "\"";
    tmp += mLabels[index];
    tmp += "\": ";
    tmp += to_string(output[index]);
    if(i != (n-1))
      tmp += ",";
  }
  tmp += "}}";

  delete []array;
  delete []output;
  return tmp;
}

void Detector::execute() {

  Mat image;

  while(true) {
    mCap->grab();
    ms_t s = getms();    
    mCap->get_frame().copyTo(image);
#ifdef DEVEL
    resize(image, image, Size(224, 224), 0, 0, INTER_LINEAR);
#endif
    image.convertTo(image, CV_32FC3);
    image = (image - 127.5)/127.5;
    mOutput = network.inference((float*)image.data);
    cout << "inference: " << getms() - s << endl;
  }

}

