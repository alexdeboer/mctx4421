EyeBotDir = /home/pi/EyeBot_BB
eyebotHeaders = -I$(EyeBotDir)/include -I$(EyeBotDir)/raspicam/robidouille/raspicam_cv
eyebotLibDir = -L$(EyeBotDir)/lib
libs = -leyebot -lX11 -ldl -lopencv_core -lopencv_highgui -lopencv_imgproc
make = gcc $(eyebotHeaders) $(eyebotLibDir)

helloWorld:
	@echo ======Making EyeBot demo======
	$(make) -o helloWorld helloWorld.c $(libs)

install: helloWorld
	sudo cp helloWorld /home/pi/EyeBot_BB/usr/helloWorld

clean:
	rm -f helloWorld
