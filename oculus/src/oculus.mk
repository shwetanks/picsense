TARGET=oculus.a

SRC=cv_functions.cpp tess_functions.cpp oculus_bridge.cpp

LIBS+=-L../../depends/lib -llept -ltesseract -lm
LIBS+=-L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -lopencv_imgcodecs

INCLUDES+=/usr/local/include/opencv2 ../include 

include ../make/cppmake.mk
