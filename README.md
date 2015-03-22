#Magoo
####Navigation + Obstacle Detection for the visually impaired

##Modules
###Navigation
- Software
	- Server
		- Install dependencies -> npm install
		- Run server.js -> node server.js
	- Spark Core
		- Create project in Spark Build IDE
		- Upload all files located in the libraries folder
- Hardware
	- Spark Core
	- Adafruit Ultimate GPS Breakout - 66 channel w/10 Hz updates - Version 3
	- Triple-axis Accelerometer+Magnetometer (Compass) Board - LSM303
	- Button
	- Coin Vibration Motor

###Obstacle Detection
- Software
	- Arduino Mini
		- Follow tutorial to upload to Arduino Mini
			- http://www.instructables.com/id/Uploading-sketch-to-Arduino-Pro-Mini-using-Arduino/
		- Flash obstacle_avoidance.ino	
- Hardware
	- Arduino Mini
	- Parallax PING))) Ultrasonic Distance Sensor
	- Coin Vibration Motor
Adafruit 
###Dependencies
- Adafruit GPS library
- Adafruit LSM303 library
- Adafruit Sensor wrapper library
- Googlemaps node module
