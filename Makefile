
DEVEL = 1

OUTDIR = objs
SRCDIR = src

SRC = $(wildcard $(SRCDIR)/*.cc)
OBJS = $(addsuffix .o, $(basename $(patsubst $(SRCDIR)/%,$(OUTDIR)/%,$(SRC))))
SRC += cpp-base64/base64.cpp


TVM_ROOT=/home/user/tvm/
DMLC_CORE=${TVM_ROOT}/3rdparty/dmlc-core

#TVM_FLAGS = -std=c++14 -O3 -fPIC
TVM_LIBS = -L${TVM_ROOT}/build/ -ltvm_runtime
TVM_INCLUDE = -I${TVM_ROOT}/include -I${DMLC_CORE}/include -I${TVM_ROOT}/3rdparty/dlpack/include

INCLUDE = -I target/include/ -I./ $(TVM_INCLUDE)
LIBS = -L./target/lib/ -lwebsockets -lssl -lcrypto `pkg-config opencv --cflags --libs` -lpthread $(TVM_LIBS)


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

endif

all: $(OUTDIR) $(OBJS)
	$(CXX) $(CXXFLAGS) $(DEFINE) $(INCLUDE) $(OBJS) $(LIBS) -o main 

$(OUTDIR)/%.o: $(SRCDIR)/%.cc 
	$(CXX) $(CXXFLAGS) $(DEFINE) $(INCLUDE) $(LIBS) -c $< -o $@ 

$(OUTDIR):
	mkdir -p $(OUTDIR)

.PHONY: clean
clean:
	rm -rf $(OUTDIR)/*
