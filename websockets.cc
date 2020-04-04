#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <sys/time.h>
#include "websockets.h"
#include <fstream>


#ifdef TFLITE

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/optional_debug_tools.h"
using namespace tflite;
std::unique_ptr<Interpreter> interpreter;

#define IM_SIZE 224*224*3

#define TFLITE_MINIMAL_CHECK(x)                              \
  if (!(x)) {                                                \
    fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
    exit(1);                                                 \
  }
#endif


#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
Mat frame(HEIGHT, WIDTH, CV_8UC3);

#ifdef DEVEL
extern VideoCapture cap;
#else
#include "raspicam.h"
extern raspicam::RaspiCam Camera;
unsigned char *data = new unsigned char[WIDTH*HEIGHT*CHANNEL];
#endif

using namespace std;
vector<string> labels;
string encoded_jpg;
vector<unsigned char> buf;
unsigned char* base64_jpg;
float *output;
#define FILENAME "labels.txt"


unsigned long long getms() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec*1.0e+3 + tv.tv_usec/1000;
}

long long prev_time = 0;
long long curr_time = 0;

enum protocols
{
  PROTOCOL_HTTP = 0,
  PROTOCOL_EXAMPLE,
  PROTOCOL_COUNT
};

struct payload
{
  unsigned char data[LWS_SEND_BUFFER_PRE_PADDING + EXAMPLE_RX_BUFFER_BYTES + LWS_SEND_BUFFER_POST_PADDING];
  size_t len;
} received_payload;

struct lws_protocols protocols[] =
{
  /* The first protocol must always be the HTTP handler */
  {
    "http-only",   /* name */
    callback_http, /* callback */
    0,             /* No per session data. */
    0,             /* max frame size / rx buffer */
  },
  {
    "preview",
    preview_callback,
    0,
    EXAMPLE_RX_BUFFER_BYTES,
  },
  { NULL, NULL, 0, 0 } /* terminator */
};


int websocket_write_back(struct lws *wsi_in, const char *str, int str_size_in) 
{
    if (str == NULL || wsi_in == NULL)
        return -1;
 
    int n;
    int len;
    unsigned char *out = NULL;
 
    if (str_size_in < 1) 
        len = strlen(str);
    else
        len = str_size_in;
 
    out = (unsigned char *)malloc(sizeof(unsigned char)*(LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING));
    //* setup the buffer*/
    memcpy (out + LWS_SEND_BUFFER_PRE_PADDING, str, len );
    //* write out*/
    n = lws_write(wsi_in, out + LWS_SEND_BUFFER_PRE_PADDING, len, LWS_WRITE_TEXT);
 
    //* free the buffer*/
    free(out);
 
    return n;
}



int callback_http( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{
	switch( reason )
	{
		case LWS_CALLBACK_HTTP:
			lws_serve_http_file( wsi, "example.html", "text/html", NULL, 0 );
			break;
		default:
			break;
	}

	return 0;
}


int find_max_index(float *array, int size) {

  float max = 0;
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

//string gen_detect_result(vector<string> labels, float *output) {
string gen_detect_result() {

  string tmp;
  int n = labels.size();
  float *array = new float[n];
  memcpy(array, output, sizeof(float)*n);


  tmp = "{\"detect\":{";
  for(int i = 0; i < n; i++) {
    int index = find_max_index(array, n);
    tmp += "\"";
    tmp += labels[index];
    tmp += "\": ";
    tmp += to_string(output[index]);
    if(i != (n -1))
      tmp += ",";
  }
  tmp += "}}";
  cout << tmp << endl;
  return tmp;
}

long long s;
int preview_callback(struct lws *wsi, enum lws_callback_reasons reason, 
  void *user, void *in, size_t len ) {
  string tmp;
  unsigned int length;

#ifdef TFLITE
  Mat image(224, 224, CV_8UC3); 
#endif
  switch(reason) {

    case LWS_CALLBACK_RECEIVE:
      //memcpy( &received_payload.data[LWS_SEND_BUFFER_PRE_PADDING], in, len );
      //received_payload.len = len;
      //lws_callback_on_writable_all_protocol( lws_get_context( wsi ), lws_get_protocol( wsi ) );
      break;

    case LWS_CALLBACK_SERVER_WRITEABLE:
      
      //lws_write( wsi, &received_payload.data[LWS_SEND_BUFFER_PRE_PADDING], received_payload.len, LWS_WRITE_TEXT );
#ifdef DEVEL
      cap.read(frame);
#else
      Camera.grab();
      //Camera.retrieve(frame);
      Camera.retrieve(data, raspicam::RASPICAM_FORMAT_RGB);
      memcpy(frame.data, data, WIDTH*HEIGHT*CHANNEL);
      //cvtColor(frame, frame, CV_RGB2BGR);
#endif

      cv::imencode(".jpg", frame, buf);
      base64_jpg = reinterpret_cast<unsigned char*>(buf.data());
      encoded_jpg = base64_encode(base64_jpg, buf.size());

      tmp = "{\"base64\":\"";
      tmp += encoded_jpg;
      tmp += "\"}";
      websocket_write_back(wsi, tmp.c_str(), -1);
      curr_time = getms();

      //cout << getms() - s << endl;
      //s = getms();
      if((curr_time - prev_time) < 3000) break;

#ifdef TFLITE
  memcpy(image.data, data, 224*224*3);
  image.convertTo(image, CV_32FC3);
  memcpy(interpreter->typed_input_tensor<float>(0), image.data, IM_SIZE*sizeof(float));
  TFLITE_MINIMAL_CHECK(interpreter->Invoke() == kTfLiteOk);
  output = interpreter->typed_output_tensor<float>(0);
#endif

      srand(time(NULL));
      for(size_t i = 0; i < labels.size(); i++)
        output[i] = (float)rand()/(RAND_MAX + 1.0);
      websocket_write_back(wsi, gen_detect_result().c_str(), -1);
      prev_time = curr_time;
 
      break;
    default:
      break;
  }

  return 0;
}


void Websockets::read_labels(void) {

#ifdef TFLITE
  std::unique_ptr<tflite::FlatBufferModel> model =
      tflite::FlatBufferModel::BuildFromFile("/home/root/model.tflite");
  TFLITE_MINIMAL_CHECK(model != nullptr);

  // Build the interpreter
  tflite::ops::builtin::BuiltinOpResolver resolver;
  InterpreterBuilder builder(*model, resolver);
  builder(&interpreter);
  TFLITE_MINIMAL_CHECK(interpreter != nullptr);

  // Allocate tensor buffers.
  TFLITE_MINIMAL_CHECK(interpreter->AllocateTensors() == kTfLiteOk);
  //tflite::PrintInterpreterState(interpreter.get());
#endif


  ifstream file(FILENAME);
  string s;
  while (std::getline(file, s)) {
    labels.push_back(s); 
  }

  output = new float[labels.size()];
  file.close();
}

Websockets::Websockets() {

  memset(&info, 0, sizeof(info));

  info.port = 8000;
  info.protocols = protocols;
  info.gid = -1;
  info.uid = -1;

  context = lws_create_context(&info);
  read_labels();
}

Websockets::~Websockets() {
  lws_context_destroy(context);
}

void Websockets::run(void) {
  while(1) {
    lws_service(context, 50);
  }
}


