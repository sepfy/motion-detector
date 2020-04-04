
DEVEL = 0

SRCS = motion.cc websockets.cc cpp-base64/base64.cpp

INCLUDE = -I target/include/
LIBS = -L target/lib/ -lwebsockets -lssl -lcrypto `pkg-config opencv --cflags --libs`

ifeq ($(DEVEL), 1)
    DEFINE += -D DEVEL
else
  PI_MMAL_INC = -I /opt/poky/2.7.3/sysroots/cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi/usr/include/interface/ -I ./raspicam/src/
  PI_MMAL_LIB = -lmmal -lmmal_core -lmmal_util -lvcos -lpthread -ldl -lvcsm -lmmal_vc_client -lmmal_components -lvchostif -lbcm_host
  INCLUDE += $(PI_MMAL_INC)
  LIBS += $(PI_MMAL_LIB)

SRCS += raspicam/src/raspicam.cpp \
        raspicam/src/private/private_impl.cpp \
        raspicam/src/private/threadcondition.cpp


  #DEFINE += -D TFLITE
  #INCLUDE += -I ./tensorflow/ -I ./tensorflow/tensorflow/lite/tools/make/downloads/absl/ -I ./tensorflow/tensorflow/lite/tools/make/downloads/flatbuffers/include/
  #LIBS += libtensorflow-lite.a -latomic

endif

PROG = motion

$(PROG): $(SRCS)
	$(CXX) -o $(PROG) $(SRCS) $(DEFINE) $(INCLUDE) $(LIBS)
