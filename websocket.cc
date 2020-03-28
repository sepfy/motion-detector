#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
using namespace std;


static int websocket_write_back(struct lws *wsi_in, const char *str, int str_size_in) 
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



static int callback_http( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
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
#define TEST "2oiu3421je2odjiwqjdoqwijd"
#define EXAMPLE_RX_BUFFER_BYTES (10)
struct payload
{
	unsigned char data[LWS_SEND_BUFFER_PRE_PADDING + EXAMPLE_RX_BUFFER_BYTES + LWS_SEND_BUFFER_POST_PADDING];
	size_t len;
} received_payload;

static int callback_example( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
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
			websocket_write_back(wsi, "test", -1);
			break;

		default:
			break;
	}

	return 0;
}

enum protocols
{
	PROTOCOL_HTTP = 0,
	PROTOCOL_EXAMPLE,
	PROTOCOL_COUNT
};

static struct lws_protocols protocols[] =
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

/*
int main(int argc, char *argv[]) {

  struct lws_context_creation_info info;
  memset(&info, 0, sizeof(info));

  info.port = 8000;
  info.protocols = protocols;
  info.gid = -1;
  info.uid = -1;

  struct lws_context *context = lws_create_context(&info);

  while(1) {
    lws_service(context, 1000000);
  }

  lws_context_destroy( context );
  return 0;
}
*/
