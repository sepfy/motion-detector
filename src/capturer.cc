#include "capturer.h"

Capturer::Capturer() {

#ifdef DEVEL
  camera_.open(0);
  if (!camera_.isOpened()) {
    std::cerr << "ERROR! Unable to open camera_\n";
    exit(-1);
  }
#else
  data = new unsigned char[width*height*channel];
  camera_.setWidth(width);
  camera_.setHeight(height);
  camera_.setFrameRate(30);
  //Camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
  //Camera.set(CV_CAP_PROP_FRAME_WIDTH, 480);
  //Camera.set(CV_CAP_PROP_FRAME_HEIGHT, 270);
  //Camera.set(CV_CAP_PROP_FPS, 30);
  //Camera.setEncoding(raspicam::RASPICAM_ENCODING_JPEG);
  camera_.open();
  frame_ = cv::Mat(224, 224, CV_8UC3);
#endif

}

Capturer::~Capturer() {

}

cv::Mat Capturer::GetFrame() {
  Grab();
  return frame_;
}

std::string Capturer::GetBase64Image() {
  Grab();
  std::vector<unsigned char> buf;
  unsigned char* base64_jpg;
  cv::imencode(".jpg", frame_, buf);
  base64_jpg = reinterpret_cast<unsigned char*>(buf.data());
  return base64_encode(base64_jpg, buf.size());
}


void Capturer::Dump(char *filename) {
  cv::imwrite(filename, frame_);
}

void Capturer::Grab() {
 
#ifdef DEVEL
  camera_.read(frame_);
#else
  camera_.grab();
  //Camera.retrieve(frame);
  camera_.retrieve(data, raspicam::RASPICAM_FORMAT_RGB);
  memcpy(frame_.data, data, width*height*channel);
#endif

}

