PWD=$(shell pwd)
TARGET=fontli

SRC+=fontli.cpp

LIBS+=-L../../oculus/lib -loculus
LIBS+=-L../../depends/lib -llept -ltesseract -lm
LIBS+=-L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -lopencv_imgcodecs

INCLUDES+=/usr/local/include
INCLUDES+=../../oculus/include ../include 

include ../../oculus/make/cppmake.mk
