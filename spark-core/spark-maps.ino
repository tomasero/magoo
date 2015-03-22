#include "Adafruit_LSM303_U.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_GPS.h"
#include "math.h"

#define mySerial Serial1
Adafruit_GPS GPS(&mySerial);
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
   
// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  false

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;

#define APP_VERSION 10

byte bufferSize = 64;
byte bufferIndex = 0;
char buffer[65];
char c;
double compass = 0;
float PI = 3.14159265359;
int motor = A0;

//inputs
int button = D2;
// int magnetometer = A0;
uint32_t timer;

void setup() {
    
    pinMode(button, INPUT);
    pinMode(motor, OUTPUT);
    // pinMode(magnetometer, INPUT);
    // if (!mag.begin()) {
    //     Spark.publish("status", "{ status: \"No magnetometer!\"}", 60, PRIVATE );
    // }
    
    // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
    GPS.begin(9600);
    mySerial.begin(9600);
    Serial.begin(115200);
    // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    // Set the update rate
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
    // For the parsing code to work nicely and have time to sort thru the data, and
    // print it out we don't suggest using anything higher than 1 Hz
    
    // Request updates on antenna status, comment out to keep quiet
    GPS.sendCommand(PGCMD_NOANTENNA);
    delay(1000);
    // Ask for firmware version
    mySerial.println(PMTK_Q_RELEASE);
    timer = millis();
    Spark.publish("status", "{ status: \"started up! "+String(APP_VERSION)+"\"}", 60, PRIVATE );
    
    IPAddress myIP = WiFi.localIP();
    Spark.publish("MY_IP", 
    String(myIP[0]) + "." + String(myIP[1]) + "." + String(myIP[2]) + "." + String(myIP[3]), 
    60, PRIVATE );
    
    //magnetometer
    mag.enableAutoRange(true);
    if(!mag.begin()) {
        Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
        while(1);
    }
    // displaySensorDetails();
    Spark.variable("compass", &compass, DOUBLE);
    Spark.function("vibrate", vibrate);
    Spark.function("getGPS", getGPS);
}

void sendGPS() {
      Spark.publish("GPS", 
      "{\"lat\": \"" + String(GPS.lat) + String(GPS.latitude)
      + "\", \"lon\": \"" + String(GPS.lon) + String(GPS.longitude) 
//      + "\", \"alt\": \"" + String(GPS.altitude)
      + "\" }"
      );
}

int pressed = 0;
int state = 0;

void handleButton() {
  pressed = digitalRead(button);
  if(state == 0 && pressed == 1) {
      Spark.publish("pressed", "press");
      sendGPS();
      state = 1;
  } else if(state == 1 && pressed == 0) {
      Spark.publish("pressed", "release");
      state = 0;
  }
}

void handleGPS() {
    // in case you are not using the interrupt above, you'll
    // need to 'hand query' the GPS, not suggested :(
    if (! usingInterrupt) {
        char c = GPS.read();    // read data from the GPS in the 'main loop'
        if (GPSECHO)            // if you want to debug, this is a good time to do it!
            if (c) Serial.print(c);
    }
    // if a sentence is received, we can check the checksum, parse it...
    if (GPS.newNMEAreceived()) {
        //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
        if (!GPS.parse(GPS.lastNMEA()))   {
          // this also sets the newNMEAreceived() flag to false
          if (millis() - timer > 10000) {
            // Spark.publish("GPS", "{ last: \""+String(GPS.lastNMEA())+"\"}", 60, PRIVATE );
            // Spark.publish("GPS", "{ error: \"failed to parse\"}", 60, PRIVATE );
          }
          return;  // we can fail to parse a sentence in which case we should just wait for another
        }
    }
    // if millis() or timer wraps around, we'll just reset it
    if (timer > millis())  timer = millis();
}

void handleMagnetometer() {
    sensors_event_t event; 
    mag.getEvent(&event);
    // Calculate the angle of the vector y,x
    compass = (atan2(event.magnetic.y, event.magnetic.x) * 180) / PI;
    // Normalize to 0-360
    if (compass < 0) {
        compass += 360;
    }
}

void loop() {
  handleGPS();
  handleButton();
  handleMagnetometer();
}

bool vibrating = false;
int vibrate(String args) {
    Spark.publish("status", "{ status:" + args +"\"}");
    if (!vibrating) {
        digitalWrite(motor, true);    
        vibrating = true;
    } else {
        digitalWrite(motor, false);    
        vibrating = false;
    }
    // digitalWrite(motor, 255);    
    return 1;
}

int getGPS(String args) {
    sendGPS();
    return 1;
}

 
 
 