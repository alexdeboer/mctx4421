#Make file for the EyeBot RoBIOS software and library
#This Makefile should be used to compile everything
#to store make output make &> output.txt
DIR = $(shell pwd)
DIR_DEMOS = /usr/Eyebot/Demos

EYEBOT_SRC_FILES_REL = adc.c camera.c globals.c hdt.c irtv.c key.c latches.c lcd.c misc.c mpsse.c multitasking.c psd_sensors.c servos_and_motors.c spi.c support.c system.c timer.c vomega.c listmenu.c
EYEBOT_SRC_FILES = $(addprefix src/, $(EYEBOT_SRC_FILES_REL))
EYEBOT_OBJ_FILES_REL = adc.o camera.o globals.o hdt.o irtv.o key.o latches.o lcd.o misc.o mpsse.o multitasking.o psd_sensors.o servos_and_motors.o spi.o support.o system.o timer.o vomega.o listmenu.o
EYEBOT_OBJ_FILES = $(addprefix src/, $(EYEBOT_OBJ_FILES_REL))

ROBIOS_SRC_FILES = robios_main.c robios_demo.c robios_exec.c robios_hdt.c robios_processes.c robios_common.c
ROBIOS_OBJ_FILES = robios_main.o robios_demo.o robios_exec.o robios_hdt.o robios_processes.o robios_common.o

HEADER_DIRS = -I$(DIR)/include -I$(DIR)/raspicam/robidouille/raspicam_cv

LIB_DIR = -L$(DIR)/lib -L$(HOME)/EyeBot_BB/raspicam/robidouille/raspicam_cv -L$(HOME)/EyeBot_BB/raspicam/userland/build/lib
LIB = -lmpsse -lX11 -pthread -llirc_client -lm -lopencv_core -lopencv_highgui -lopencv_imgproc -lraspicamcv -lmmal_core -lmmal -lmmal_util -lvcos -lbcm_host -lImlib2
#Info on the different libraries used
#mpsse: (-lmpsse)
#X11: X server, used for the GUI (-lX11)
#pthread: used for threads (-pthread)
#lirc_client: IR input library (-llirc_client)
#m: the c math library (-lm)
#opencv_core: the core opencv library (-lopencv_core)
#opencv_highgui: opencv gui and media IO library (-lopencv_highgui)
#opencv_imgproc: opencv image processing library (-lopencv_imgproc)

COMPILER_OPTIONS = $(HEADER_DIRS)
COMPILER_DEFINES = 

COMPILER =  gcc $(COMPILER_OPTIONS) $(COMPILER_DEFINES) $(LIB_DIR)

all: ROBIOS_ALL DEMOS

remake: clean all

clean: 
	cd $(DIR)/src; make clean
	cd $(DIR)/lib; rm libeyebot.a
	cd $(DIR)/demos; make clean
	rm -f $(EYEBOT_OBJ_FILES) $(ROBIOS_OBJ_FILES) EyeBot

ROBIOS_ALL: EyeBotLib $(ROBIOS_OBJ_FILES)
	@echo ======Compiling ROBIOS software======
	$(COMPILER) -o EyeBot $(EYEBOT_OBJ_FILES) $(ROBIOS_OBJ_FILES) $(LIB)
	
ROBIOS_ONLY: $(ROBIOS_OBJ_FILES)
	cd $(DIR)/src; make
	@echo ======Compiling ROBIOS software======
	$(COMPILER) -o EyeBot $(EYEBOT_OBJ_FILES) $(ROBIOS_OBJ_FILES) $(LIB)

EyeBotLib: 
	@echo ======Compiling EyeBot library======
	cd $(DIR)/src; make
	ar rcs lib/libeyebot.a $(EYEBOT_OBJ_FILES)
	ranlib lib/libeyebot.a

DEMOS:
	@echo ======Making EyeBot Demos======
	cd $(DIR)/demos; make

install: all 
	cd $(DIR)/demos; make install

robios_main.o: robios_main.c header.h include/eyebot.h
	$(COMPILER) -c -o robios_main.o robios_main.c
	
robios_demo.o: robios_demo.c header.h include/eyebot.h
	$(COMPILER) -c -o robios_demo.o robios_demo.c
	
robios_exec.o: robios_exec.c header.h include/eyebot.h
	$(COMPILER) -c -o robios_exec.o robios_exec.c
	
robios_hdt.o: robios_hdt.c header.h include/eyebot.h
	$(COMPILER) -c -o robios_hdt.o robios_hdt.c
	
robios_processes.o: robios_processes.c header.h include/eyebot.h
	$(COMPILER) -c -o robios_processes.o robios_processes.c

robios_common.o: robios_common.c header.h include/eyebot.h
	$(COMPILER) -c -o robios_common.o robios_common.c
