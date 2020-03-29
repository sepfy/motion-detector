
DEVEL = 0

SRCS = motion.cc websockets.cc cpp-base64/base64.cpp

INCLUDE = -I target/include/
LIBS = -L target/lib/ -lwebsockets -lssl -lcrypto

ifeq ($(DEVEL), 1)
    DEFINE += -D DEVEL
    LIBS += `pkg-config opencv --cflags --libs`
else
  PI_MMAL_INC = -I /opt/poky/2.7.3/sysroots/cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi/usr/include/interface/ -I ./raspicam/src/
  PI_MMAL_LIB = -lmmal -lmmal_core -lmmal_util -lvcos -lpthread -ldl -lvcsm -lmmal_vc_client -lmmal_components -lvchostif -lbcm_host

  INCLUDE += $(PI_MMAL_INC)
  LIBS += $(PI_MMAL_LIB)

SRCS += raspicam/src/raspicam_still.cpp \
        raspicam/src/private_still/private_still_impl.cpp
endif

PROG = motion

$(PROG): $(SRCS)
	$(CXX) -o $(PROG) $(SRCS) $(DEFINE) $(INCLUDE) $(LIBS)
