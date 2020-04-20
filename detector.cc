#include "detector.h"

Detector::Detector() {

}

Detector::~Detector() {

}

void Detector::Init(char *labelname, char *modelname, Capturer *cap) {

  ifstream file(labelname);
  string s;
  while (std::getline(file, s)) {
    mLabels.push_back(s); 
  }
  file.close();

  mOutput = new float[mLabels.size()];

  network.load(modelname, 1);

  mCap = cap;

  string cmd = "mkdir -p " + mDir;
  system(cmd.c_str());
  int index = GetDirIndex();
  mDir = mDir + "/Dumps_" + to_string(index);
  cmd = "mkdir -p " + mDir;
  system(cmd.c_str());
}

int Detector::GetDirIndex() {

  struct dirent *dp;
  DIR *fd;
  if ((fd = opendir(mDir.c_str())) == NULL) {
    fprintf(stderr, "listdir: can't open %s\n", mDir.c_str());
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
  int n = mLabels.size();
  float *array = new float[n];
  float *output = new float[n];
  memcpy(array, mOutput, sizeof(float)*n);
  memcpy(output, mOutput, sizeof(float)*n);

  tmp = "{\"detect\":{";
  for(int i = 0; i < n; i++) {
    int index = FindMaxIndex(array, n);
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

void Detector::Execute() {

  Mat image;
  int count = 0;
  while(true) {
    mCap->Grab();
    ms_t s = getms();    
    mCap->GetFrame().copyTo(image);
#ifdef DEVEL
    resize(image, image, Size(224, 224), 0, 0, INTER_LINEAR);
#endif
    image.convertTo(image, CV_32FC3);
    image = (image - 127.5)/127.5;
    mOutput = network.inference((float*)image.data);
    cout << "inference: " << getms() - s << endl;
    float max = 0;
    int max_idx = -1;
    for(int i = 0; i < 3; i++) {
      if(mOutput[i] > max) {
        max = mOutput[i];
        max_idx = i;
      }
      cout << "Class " << i << ": " << mOutput[i] << endl;
    }
    if(max_idx == 0) {
      char buf[32] = {0};
      sprintf(buf, "%s/capture_%d.jpg", mDir.c_str(), count);
      mCap->Dump(buf);
      count++;
    }
    break;
  }
}

