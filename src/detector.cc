#include "detector.h"

using namespace std;
using namespace cv;

void Detector::Init(char *labelname, Capturer *cap) {

  capturer_ = cap;


  ifstream file(labelname);
  string s;
  while (std::getline(file, s)) {
    labels_.push_back(s); 
  }
  file.close();

  output_ = new float[labels_.size()];
  input_ = new float[kWidth*kHeight*kChannel];

  string create_root_dir_cmd = "mkdir -p " + storage_dir_;
  system(create_root_dir_cmd.c_str());

  storage_dir_ = storage_dir_ + "/Dumps_" + to_string(GetDirIndex());
  string create_dump_dir_cmd = "mkdir -p " + storage_dir_;
  system(create_dump_dir_cmd.c_str());

}

int Detector::GetDirIndex() {

  struct dirent *dp;
  DIR *fd;
  if ((fd = opendir(storage_dir_.c_str())) == NULL) {
    fprintf(stderr, "listdir: can't open %s\n", storage_dir_.c_str());
    exit(0);
  }

  int max = -1;
  while ((dp = readdir(fd)) != NULL) {
  if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
    continue;
    //cout << dp->d_name << endl;
    //Follow the rules of folder name. Dumps/Dumps_XXX
    if(atoi(dp->d_name+6) > max)
      max = atoi(dp->d_name+6);
  }
  max++;
  return max;

}

int Detector::FindMaxIndex(float *array, int size) {

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


string Detector::GetResult() {

  string tmp;
  int n = labels_.size();
  float *array = new float[n];
  float *output = new float[n];
  memcpy(array, output_, sizeof(float)*n);
  memcpy(output, output_, sizeof(float)*n);

  tmp = "{\"detect\":{";
  for(int i = 0; i < n; i++) {
    int index = FindMaxIndex(array, n);
    tmp += "\"";
    tmp += labels_[index];
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

void Detector::Preprocess(cv::Mat image) {

#ifdef DEVEL
    resize(image, image, Size(224, 224), 0, 0, INTER_LINEAR);
#endif
    image.convertTo(image, CV_32FC3);
    memcpy(input_, image.data, kWidth*kHeight*kChannel*sizeof(float));
}

int Detector::ArgmaxOutput() {
  return distance(output_, max_element(output_, output_ + labels_.size()));
}


void Detector::Execute() {

  Mat image;
  int count = 0;

  while(true) {
    if(!detecting_)
      continue;
    capturer_->Grab();
    capturer_->GetFrame().copyTo(image);
    Preprocess(image);
    Detect();

    int best = ArgmaxOutput();
    std::cout << best << endl;
    if(best == -1) {
      char buf[32] = {0};
      sprintf(buf, "%s/capture_%d.jpg", storage_dir_.c_str(), count);
      capturer_->Dump(buf);
      count++;
    }
  }
}

