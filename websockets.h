#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
using namespace std;

#define EXAMPLE_RX_BUFFER_BYTES (10)

int websocket_write_back(struct lws *wsi_in, const char *str, int str_size_in); 
static int callback_http( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
int preview_callback( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len );

int images_callback( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len );

class Websockets { 
  public:
    Websockets();
    ~Websockets();
   void Listen(void);

  private:
    struct lws_context_creation_info info;
    struct lws_context *context;
};

