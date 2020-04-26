
DEVEL = 1

SRCS = src/motion.cc src/capturer.cc src/detector.cc src/detector_factory.cc src/detector_tvm.cc cpp-base64/base64.cpp

TVM_ROOT=/home/chunyu/tvm/
DMLC_CORE=${TVM_ROOT}/3rdparty/dmlc-core

TVM_FLAGS = -std=c++14 -O3 -fPIC\
  -I${TVM_ROOT}/include\
  -I${DMLC_CORE}/include\
  -I${TVM_ROOT}/3rdparty/dlpack/include\
  -L${TVM_ROOT}/build/ -ltvm_runtime


INCLUDE = -I target/include/ -I XNOR-Net-Core/include/ -I XNOR-Net-Core/gemmbitserial/ -I./
LIBS = target/lib/libwebsockets.a XNOR-Net-Core/libxnnc.a -lssl -lcrypto `pkg-config opencv --cflags --libs` -lpthread -lcap

ifeq ($(DEVEL), 1)
    DEFINE += -D DEVEL
else
  PI_MMAL_INC = -I /opt/poky/2.7.3/sysroots/cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi/usr/include/interface/ -I ./raspicam/src/
  PI_MMAL_LIB = -lmmal -lmmal_core -lmmal_util -lvcos -lpthread -ldl -lvcsm -lmmal_vc_client -lmmal_components -lvchostif -lbcm_host
  INCLUDE += $(PI_MMAL_INC)
  LIBS += $(PI_MMAL_LIB) -lcap

SRCS += raspicam/src/raspicam.cpp \
        raspicam/src/private/private_impl.cpp \
        raspicam/src/private/threadcondition.cpp


  #DEFINE += -D TFLITE
  #INCLUDE += -I ./tensorflow/ -I ./tensorflow/tensorflow/lite/tools/make/downloads/absl/ -I ./tensorflow/tensorflow/lite/tools/make/downloads/flatbuffers/include/
  #LIBS += libtensorflow-lite.a -latomic

endif

PROG = motion

$(PROG): $(SRCS)
	$(CXX) -std=c++11 -o $(PROG) $(SRCS) $(DEFINE) $(INCLUDE) $(LIBS) $(TVM_FLAGS)

clean:
	rm -rf $(PROG)
