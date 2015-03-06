EYEBOTDIR = /home/pi/EyeBot_BB

DIRS = -I$(EYEBOTDIR)/include -L$(EYEBOTDIR)/lib -I$(EYEBOTDIR)/raspicam/robidouille/raspicam_cv -L$(EYEBOTDIR)/raspicam/robidouille/raspicam_cv -L$(EYEBOTDIR)/raspicam/userland/build/lib
LIBS = -leyebot -lopencv_core -lopencv_highgui -lopencv_imgproc -lraspicamcv -lmmal_core -lmmal -lmmal_util -lvcos -lbcm_host

MAKE = gcc $(DIRS)

all: 
	$(MAKE) -o cameraDemo main.c $(LIBS)
