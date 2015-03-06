EyeBotDir = /home/pi/EyeBot_BB
eyebotHeaders = -I$(EyeBotDir)/include -I/home/pi/Eyebot_BB -I$(EyeBotDir)/raspicam/robidouille/raspicam_cv
eyebotLibDir = -L$(EyeBotDir)/lib
libs = -leyebot -ldl -llirc_client -lX11 -lopencv_core -lopencv_highgui -lopencv_imgproc
make = gcc $(eyebotHeaders) $(eyebotLibDir)

eyebotDemo:
	@echo ======Making EyeBot IR Demo======
	$(make) -o irDemo irDemo.c $(libs)

install: eyebotDemo
	sudo cp irDemo /home/pi/EyeBot_BB/usr/Demos

clean: 
	rm -f irDemo
