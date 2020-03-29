
DEVEL = 1

SRCS = motion.cc websockets.cc cpp-base64/base64.cpp

INCLUDE = -I target/include/
LIBS = -L target/lib/ -lwebsockets -lssl -lcrypto

ifeq ($(DEVEL), 1)
    DEFINE += -D DEVEL
    LIBS += `pkg-config opencv --cflags --libs`
endif

PROG = motion

$(PROG): $(SRCS)
	$(CXX) -o $(PROG) $(SRCS) $(DEFINE) $(INCLUDE) $(LIBS)
