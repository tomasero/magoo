// This #include statement was automatically added by the Spark IDE.
#include "Adafruit_GPS.h"
#include <math.h>

#define mySerial Serial1
Adafruit_GPS GPS(&mySerial);


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

uint32_t timer;

void setup() {
      pinMode(D1, INPUT);

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



  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!


  delay(1000);
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);
  timer = millis();
  Spark.publish("GPS", "{ status: \"started up! "+String(APP_VERSION)+"\"}", 60, PRIVATE );

  IPAddress myIP = WiFi.localIP();
  Spark.publish("MY_IP",
    String(myIP[0]) + "." + String(myIP[1]) + "." + String(myIP[2]) + "." + String(myIP[3]),
    60, PRIVATE );
}

void sendGPS() {
      Spark.publish("GPS",
      "{lat: " + String(GPS.lat) + String(GPS.latitude)
      + ", lon: " + String(GPS.lon) + String(GPS.longitude)
      + ", alt: " + String(GPS.altitude)
      + " }"
      );
}

int button = 0;
int state = 0;

void handleButton() {
     button = digitalRead(D1);
  if(state == 0 && button == 1) {
      //Spark.publish("button", "press");
      sendGPS();
      state = 1;
  } else if(state == 1 && button == 0) {
    //   Spark.publish("button", "release");
      state = 0;
  }
}

void handleGPS() {

   // in case you are not using the interrupt above, you'll
  // need to 'hand query' the GPS, not suggested :(

    if (! usingInterrupt) {
    // read data from the GPS in the 'main loop'
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    if (GPSECHO)
      if (c) Serial.print(c);
  }

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
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

void loop() {
  handleGPS();
  handleButton();
}
