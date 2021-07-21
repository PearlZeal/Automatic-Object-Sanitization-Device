//  using the switch the device can be switched ON or OFF... the object is sanitized by using a servo motor pulling the trigger
//
// Copyright 2015 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// FirebaseDemo_ESP8266 is a sample that demo the different functions
// of the FirebaseArduino API.
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Servo.h>

#define echoPin D3 
#define trigPin D4
#define motorPin1 D5
#define servoPin D6
#define enb D7

Servo servo;
// firebase info.
#define FIREBASE_HOST "security-d871f.firebaseio.com"
#define FIREBASE_AUTH "WlNJDkoO6QNVfrVcYlXG0xO7V0xn2O3zJ9z3UrPe"
#define WIFI_SSID "D-Silva Link"
#define WIFI_PASSWORD "prince_2014"

int spray;
int spray_status;
char led = ' ';


void setup()
{ 
  Serial.begin(9600);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  
  pinMode(motorPin1, OUTPUT);
  servo.attach(servoPin);
  servo.write(0);
  pinMode(enb, OUTPUT);
 // pinMode(led, OUTPUT);
 Serial.println("Enter y to ON and n to OFF the motor");
}

int n = 0;

void loop()
{
  /*
   * values to firebase
   * motor on or off --> led --> button
   * ultrasonic inches --> 
   * 
   * */
  long duration, inches, cm;
 
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(enb, HIGH);
  digitalWrite(motorPin1, HIGH);
  Serial.print("motor on---- normal operation\n");
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH pulse
  // whose duration is the time (in microseconds) from the sending of the ping
  // to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);

  // convert the time into a distance
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);

  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  
  if(inches < 6)
  {
    //wait and then stop the rotating motor
      delay(250);
      digitalWrite(motorPin1, LOW);
      sprayMotor();
      Serial.print("obj detected --- spray motor ON and conyevor belt OFF\n");
      spray_status = update_spray_motor(1);
      //wait for 5 microseconds and then start the rotating motor so that sanitization takes place in that time
      delay(1000);
      digitalWrite(motorPin1, HIGH);
      Serial.print("obj detected --- spray motor OFF and conveyor belt ON\n");
      spray_status = update_spray_motor(0);
  }
  
  else
  {  
    Serial.print("obj not detected & spray motor off\n");
  }
  
  /********firebase*****/
// set value for ultrasonic sensor
  Firebase.setFloat("uss", inches);
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(1000);  //ir is for the lvl of liquid/sanitizer present
  Firebase.setFloat("ir", 0);
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(1000);
  
}

void sprayMotor()
{
  servo.write(270);
  delay(1000);
  servo.write(0);
  delay(1000);
}

long microsecondsToInches(long microseconds) 
{
  // According to Parallax's datasheet for the PING))), there are 73.746
  // microseconds per inch (i.e. sound travels at 1130 feet per second).
  // This gives the distance travelled by the ping, outbound and return,
  // so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) 
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}

int update_spray_motor(int spray)
{
  // set value for motor
  Firebase.setFloat("led", spray);
  Firebase.setFloat("uss", 0);
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return 0;
  }
  delay(1000);
  return spray;
}
