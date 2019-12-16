CC = g++
CFLAGS = -g -Wall
SRCS = motion.cc
PROG = motion
INC = -I include/
OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV) libxnnc.a -fopenmp

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(INC) $(LIBS)
