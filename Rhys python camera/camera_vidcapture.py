#Attempt at python scripted RPi camera streaming
#Working with over 9 frames per second
#Low resolutions provide much higher fps rates
#Resolution required for eye-bots is assumed low, but can be tweaked

import picamera
import picamera.array
import time
import cv2

window_name = "Eyebot Video Stream"
res_x = 320 #x-value of camera resolution
res_y = 240 #y-value of camera resolution
win_x = 400 #x-value of the TOP-LEFT corner of the video window
win_y = 100 #y-value of the TOP-LEFT corner of the video window

#initialiseCamera will allow the easy adjustment of any camera settings
def initialiseCamera(camera):
	camera.resolution = (res_x, res_y) 
	#Create and adjust the video stream viewing window 
	cv2.namedWindow(window_name) 
	cv2.moveWindow(window_name, win_x, win_y)
	cv2.resizeWindow(window_name, res_x, res_y)
	return camera
	
def main():
	with picamera.PiCamera() as camera:
		camera = initialiseCamera(camera)
		#Allow the camera to adjust to settings
		time.sleep(2)
		while True:
			with picamera.array.PiRGBArray(camera) as frame:
				#Capture the next frame
				camera.capture(frame, format='bgr', use_video_port=True)
				image = frame.array
				
				#At this point, image is a res_x by res_y by 3 BGR-valued array, eg 160x120x3 full of BGR values
				#This is where any image processing code should go
				
				#If there is a frame to show, show it
				if image is not None:
					cv2.imshow(window_name, image)
				#Escape control
				if cv2.waitKey(1) == 27:
					cv2.destroyAllWindows()
					break

main()