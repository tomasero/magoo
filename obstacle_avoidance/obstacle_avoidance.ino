
const int trigPin = 4;
const int echoPin = 2;
const int motorPin = 3;
int a;
 
void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  pinMode(motorPin, OUTPUT);
}
 
void loop()
{
  // establish variables for duration of the ping, 
  // and the distance result in inches and centimeters:
  long duration, cm;
 
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // convert the time into a distance
  
  cm = microsecondsToCentimeters(duration);
  //Serial.print(cm);
  //Serial.println();
  a=cm;
 
 
  
  if (a>20 && a<150) {
    digitalWrite(motorPin, HIGH);
    Serial.println("Vibrate");
  } else {
    digitalWrite(motorPin, LOW);
    Serial.println("Don't Vibrate");
  }
  //Serial.print(cm);
  //Serial.println();
  
  delay(200);
}
 

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
