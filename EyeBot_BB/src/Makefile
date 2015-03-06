#Makefile for the EyeBot library source
#Note: this will not make the library file only the object files,
#	to make the library file run the makefile in the previous 
#	directory
DIR = $(shell pwd)

COMPILER_OPTIONS = 
COMPILER_DEFINES = 

SRC_FILES = adc.c camera.c globals.c hdt.c irtv.c key.c latches.c lcd.c misc.c mpsse.c multitasking.c psd_sensors.c servos_and_motors.c spi.c support.c system.c timer.c vomega.c listmenu.c
OBJ_FILES = adc.o camera.o globals.o hdt.o irtv.o key.o latches.o lcd.o misc.o mpsse.o multitasking.o psd_sensors.o servos_and_motors.o spi.o support.o system.o timer.o vomega.o listmenu.o
INC_DIR = -I$(DIR)/../include -I/usr/include/opencv -I/usr/include/opencv2 -I$(DIR)/../raspicam/robidouille/raspicam_cv

COMPILER = gcc $(COMPILER_OPTIONS) $(COMPILER_DEFINES) $(INC_DIR)

source: $(OBJ_FILES)

clean: 
	rm -f $(OBJ_FILES)

adc.o: adc.c
	$(COMPILER) -c -o adc.o adc.c
	
camera.o: camera.c
	$(COMPILER) -c -o camera.o camera.c

globals.o: globals.c
	$(COMPILER) -c -o globals.o globals.c
	
hdt.o: hdt.c
	$(COMPILER) -c -o hdt.o hdt.c
	
irtv.o: irtv.c
	$(COMPILER) -c -o irtv.o irtv.c
	
key.o: key.c
	$(COMPILER) -c -o key.o key.c
	
latches.o: latches.c
	$(COMPILER) -c -o latches.o latches.c
	
lcd.o: lcd.c
	$(COMPILER) -c -o lcd.o lcd.c
	
misc.o: misc.c
	$(COMPILER) -c -o misc.o misc.c
	
mpsse.o: mpsse.c
	$(COMPILER) -c -o mpsse.o mpsse.c
	
multitasking.o: multitasking.c
	$(COMPILER) -c -o multitasking.o multitasking.c
	
psd_sensors.o: psd_sensors.c
	$(COMPILER) -c -o psd_sensors.o psd_sensors.c
	
servos_and_motors.o: servos_and_motors.c
	$(COMPILER) -c -o servos_and_motors.o servos_and_motors.c
	
spi.o: spi.c
	$(COMPILER) -c -o spi.o spi.c
	
support.o: support.c
	$(COMPILER) -c -o support.o support.c
	
system.o: system.c
	$(COMPILER) -c -o system.o system.c
	
timer.o: timer.c
	$(COMPILER) -c -o timer.o timer.c
	
vomega.o: vomega.c
	$(COMPILER) -c -o vomega.o vomega.c
	
listmenu.o: listmenu.c
	$(COMPILER) -c -o listmenu.o listmenu.c
