#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <sys/time.h>
#include <fstream>
#include "websockets.h"
#include "detector.h"

using namespace std;

extern Capturer capturer;
extern Detector detector;

clock_t curr_time = 0;
clock_t prev_time = 0;


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
  {
    "images",
    images_callback,
    0,
    16,
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

long long s;
int preview_callback(struct lws *wsi, enum lws_callback_reasons reason, 
  void *user, void *in, size_t len ) {

  string tmp;
  string encoded_jpg;

  switch(reason) {
    case LWS_CALLBACK_RECEIVE:
      //memcpy( &received_payload.data[LWS_SEND_BUFFER_PRE_PADDING], in, len );
      //received_payload.len = len;
      //lws_callback_on_writable_all_protocol( lws_get_context( wsi ), lws_get_protocol( wsi ) );
      break;

    case LWS_CALLBACK_SERVER_WRITEABLE:
      detector.Stop();
      //lws_write( wsi, &received_payload.data[LWS_SEND_BUFFER_PRE_PADDING], received_payload.len, LWS_WRITE_TEXT );
      capturer.Grab();
      encoded_jpg = capturer.GetBase64Image();
      tmp = "{\"base64\":\"";
      tmp += encoded_jpg;
      tmp += "\"}";
      websocket_write_back(wsi, tmp.c_str(), -1);

      /*
      curr_time = clock();
      if((curr_time - prev_time)/CLOCKS_PER_SEC > 2) {
        websocket_write_back(wsi, detector.GetResult().c_str(), -1);
        prev_time = curr_time;
      }
      */
      break;
    case LWS_CALLBACK_CLOSED:
      detector.Start();
    default:
      break;

  
  }

  return 0;
}

int images_callback(struct lws *wsi, enum lws_callback_reasons reason, 
  void *user, void *in, size_t len ) {

  char *buf;
  struct dirent *dp;
  DIR *fd;
  char loc[64] = {0};
  string ret = "{\"files\":[\"";
  switch(reason) {

    case LWS_CALLBACK_RECEIVE:
      buf = (char*)malloc(LWS_SEND_BUFFER_PRE_PADDING + len);
      memset(buf, 0, LWS_SEND_BUFFER_PRE_PADDING + len);
      memcpy(buf, in, len);
      sprintf(loc, "/var/www/html/%s", buf);
      if ((fd = opendir(loc)) == NULL) {
        fprintf(stderr, "listdir: can't open %s\n", loc);
        exit(0);
      }

      while ((dp = readdir(fd)) != NULL) {
        if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
          continue;
        //cout << dp->d_name;
	ret += dp->d_name;
	ret += "\",\"";
      }

      ret += "..\"]}";
      //cout << ret << endl;
      free(buf);
      websocket_write_back(wsi, ret.c_str(), -1);
      break;

    case LWS_CALLBACK_SERVER_WRITEABLE:
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

void Websockets::Listen(void) {
  lws_service(context, 50);
}


