#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <sys/time.h>
#include "websockets.h"
#include <fstream>


#ifdef DEVEL

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
extern VideoCapture cap;
Mat frame;

#else

#include "raspicam_still.h"
extern raspicam::RaspiCam_Still Camera;
unsigned char *data = new unsigned char[480*270*3];

#endif

using namespace std;
vector<string> labels;
string encoded_png;
vector<unsigned char> buf;
unsigned char* base64_png;

#define FILENAME "labels.txt"


unsigned long long getms() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec*1.0e+3 + tv.tv_usec/1000;
}

long long prev_time = 0;
long long curr_time = 0;

string encoded_png;
vector<uchar> buf;
unsigned char* base64_png;

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


int preview_callback(struct lws *wsi, enum lws_callback_reasons reason, 
  void *user, void *in, size_t len ) {

  string tmp;
  unsigned int length;
  switch(reason) {

    case LWS_CALLBACK_RECEIVE:
      memcpy( &received_payload.data[LWS_SEND_BUFFER_PRE_PADDING], in, len );
      received_payload.len = len;
      lws_callback_on_writable_all_protocol( lws_get_context( wsi ), lws_get_protocol( wsi ) );
      break;

    case LWS_CALLBACK_SERVER_WRITEABLE:
      
      //lws_write( wsi, &received_payload.data[LWS_SEND_BUFFER_PRE_PADDING], received_payload.len, LWS_WRITE_TEXT );
#ifdef DEVEL
      cap.read(frame);
      cv::imencode(".png", frame, buf);
      base64_png = reinterpret_cast<unsigned char*>(buf.data());
      encoded_png = base64_encode(base64_png, buf.size());
#else
      length = Camera.getImageBufferSize(); // Header + Image Data + Padding
      if (!Camera.grab_retrieve(data, length) ) {
        cerr<<"Error in grab"<<endl;
        return -1;
      }
      encoded_png = base64_encode(data, length);
#endif

      tmp = "{\"base64\":\"";
      tmp += encoded_png;
      tmp += "\"}";
      websocket_write_back(wsi, tmp.c_str(), -1);

      curr_time = getms();
      if((curr_time - prev_time) < 3000) break;

      srand(time(NULL));
      tmp = "{\"detect\":[";
      for(size_t i = 0; i < labels.size(); i++) {
        tmp += to_string((float)rand()/(RAND_MAX + 1.0));
        if(i != labels.size() -1)
          tmp += ",";
      }

      tmp += "]}";
      cout << tmp << endl;
      websocket_write_back(wsi, tmp.c_str(), -1);
      prev_time = curr_time;
 
      break;
    default:
      break;
  }

  return 0;
}


void Websockets::read_labels(void) {

  ifstream file(FILENAME);
  string s;
  while (std::getline(file, s)) {
    labels.push_back(s); 
  }
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
    lws_service(context, 1000000);
  }
}


