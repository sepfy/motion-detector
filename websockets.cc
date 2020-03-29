#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include "websockets.h"

#ifdef DEVEL
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
extern VideoCapture cap;
Mat frame;
#endif

using namespace std;

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
    "example-protocol",
    callback_example,
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



int callback_example( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{
	switch( reason )
	{
		case LWS_CALLBACK_RECEIVE:
			memcpy( &received_payload.data[LWS_SEND_BUFFER_PRE_PADDING], in, len );
			received_payload.len = len;
			lws_callback_on_writable_all_protocol( lws_get_context( wsi ), lws_get_protocol( wsi ) );
			break;

		case LWS_CALLBACK_SERVER_WRITEABLE:
      
			//lws_write( wsi, &received_payload.data[LWS_SEND_BUFFER_PRE_PADDING], received_payload.len, LWS_WRITE_TEXT );
                        //imshow("frame", frame);
                        //waitKey(1);
#ifdef DEVEL
      cap.read(frame);
      cv::imencode(".png", frame, buf);
      base64_png = reinterpret_cast<unsigned char*>(buf.data());
      encoded_png = base64_encode(base64_png, buf.size());
			websocket_write_back(wsi, encoded_png.c_str(), -1);
#else
			websocket_write_back(wsi, "test", -1);
#endif
			break;

		default:
			break;
	}

	return 0;
}


Websockets::Websockets() {

  memset(&info, 0, sizeof(info));

  info.port = 8000;
  info.protocols = protocols;
  info.gid = -1;
  info.uid = -1;

  context = lws_create_context(&info);

}

Websockets::~Websockets() {
  lws_context_destroy(context);
}

void Websockets::run(void) {
  while(1) {
    lws_service(context, 1000000);
  }
}


