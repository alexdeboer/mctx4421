EyeBotDir = /home/pi/EyeBot_BB
eyebotHeaders = -I$(EyeBotDir)/include -I$(EyeBotDir)/raspicam/robidouille/raspicam_cv
eyebotLibDir = -L$(EyeBotDir)/lib
libs = -leyebot -lX11 -ldl -lopencv_core -lopencv_highgui -lopencv_imgproc
make = gcc $(eyebotHeaders) $(eyebotLibDir)

eyebotDemo:
	@echo ======Making EyeBot demo======
	$(make) -o eyebotDemo eyebotDemo.c $(libs)

install: eyebotDemo
	sudo cp eyebotDemo /home/pi/EyeBot_BB/usr/Demos
	sudo cp eyebotDemo /home/pi/EyeBot_BB/usr/Software

clean:
	rm -f eyebotDemo
