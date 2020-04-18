#include "capturer.h"


Capturer::Capturer() {

#ifdef DEVEL
  camera.open(0);
  if (!camera.isOpened()) {
    cerr << "ERROR! Unable to open camera\n";
    exit(-1);
  }
#else
  data = new unsigned char[width*height*channel];
  camera.setWidth(width);
  camera.setHeight(height);
  camera.setFrameRate(30);
  //Camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
  //Camera.set(CV_CAP_PROP_FRAME_WIDTH, 480);
  //Camera.set(CV_CAP_PROP_FRAME_HEIGHT, 270);
  //Camera.set(CV_CAP_PROP_FPS, 30);
  //Camera.setEncoding(raspicam::RASPICAM_ENCODING_JPEG);
  camera.open();
  mFrame = Mat(224, 224, CV_8UC3);
#endif

}

Capturer::~Capturer() {

}

Mat Capturer::get_frame() {
  return mFrame;
}

string Capturer::get_base64() {
  vector<unsigned char> buf;
  unsigned char* base64_jpg;
  imencode(".jpg", mFrame, buf);
  base64_jpg = reinterpret_cast<unsigned char*>(buf.data());
  return base64_encode(base64_jpg, buf.size());
}

void Capturer::start() {
  bRunning = true;
}

void Capturer::stop() {
  bRunning = false;
}

void Capturer::grab() {
 
#ifdef DEVEL
  camera.read(mFrame);
#else
  camera.grab();
  //Camera.retrieve(frame);
  camera.retrieve(data, raspicam::RASPICAM_FORMAT_RGB);
  memcpy(mFrame.data, data, width*height*channel);
  //cvtColor(frame, frame, CV_RGB2BGR);
#endif
  imageReady = true;
}

