/*
 Copyright (C) 2017 LeRoy Miller.
 Control Small robot using MQTT.
 Control Codes:
 0 Stop
 1 Forward
 2 Backward
 3 Right Forward (left motor on)
 4 Left Forward (right motor on)
 5 Right Backward (left motor on)
 6 Left Backward (right motor on)

 Robot publishes status, and ultrasonic distance readings.

 Because of slowness of MQTT robot will stop itself if object is in 
 the way. 

 This is a experiment using MQTT. Currently using a public broker,
 setting can be changed below.
 Libraries required:
 ESP8266Wifi, PubSubClient, TimedAction

  License:  
 *  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses>
 
*/

/* July 3 Need to Add
 *  Speed Control
 *  IsRunning Flag (timer for motor)
 * (done) ultrasonic avarging
 * ultrasonic avarging is not used, however code was added, it didn't 
 * appear to work correctly so it's commented out.
 * (done) ultrasonic object detetion with stop code * 
 *  *** See Updated Compact Rover Ultrasonic with timedaction code ***
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <TimedAction.h>

// Update these with values suitable for your network.

const char* ssid = "Motel6";
const char* password = "";

//MQTT Variables
const char* mqtt_server = "test.mosquitto.org";
char* outTopic = "robotout";
char* inTopic = "robotin";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

//UltraSonic variables
#define TRIGGER 12 //D6
#define ECHO    14 //D5
#define MAX_DISTANCE 200
float duration, distance, distancein;

//Motor Driver Board
int left = 2; //D4
int right = 0; //D3
int leftpwm = 4; //D2
int rightpwm = 5; //D1
int setspeed = 500; //max for ESP8266 is 1023
int pingavg = 0;
int pingcount = 0;

//Timed Action Setup
void ultrasonic();
void publishmsg();
void checksonar();
TimedAction updatesonic(15, ultrasonic); //Count and make Avg Ping times
TimedAction updatepub(500, publishmsg); // Publish status and pings
TimedAction checkdistance(15, checksonar); //determine too close to object

char *robotStatus;

void setup() {
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(left, OUTPUT);
pinMode(right, OUTPUT);
pinMode(leftpwm, OUTPUT);
pinMode(rightpwm, OUTPUT);
stop();
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

switch ((char)payload[0]) {
  case '0':
  check();
  stop(); //stop motor
  break;
  case '1':
  check();
  forward(); //drive forward
  break;
  case '2':
  check();
  backward(); //drive backward
  break;
  case '3':
  check();
  forwardleft(); //turn right forward
  break;
  case '4':
  check();
  forwardright(); //turn left forward
  break;
  case '5':
  check();
  backleft(); //turn right backward
  break;
  case '6':
  check();
  backright(); //turn left backward
  break;
  default:
  robotStatus = "ERR Stop";
  check();
  stop();
}
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "Robot Online!");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
 check();
   
}

void check() {
  updatesonic.check();
  updatepub.check();
  checkdistance.check();
}

void ultrasonic() {
  digitalWrite(TRIGGER, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = (duration/2) / 29.1;
  distancein = distance * 0.39;
  pingcount ++;
  if (distancein >= MAX_DISTANCE || distancein <= 0) {
    pingavg = 0;
    pingcount = 0;
  } else {
    pingavg = pingavg + distancein;
  }
  Serial.print("Avg: ");
  Serial.print(pingavg);
  Serial.print("in. Inch: ");
  Serial.print(distancein);
  Serial.print(" Centimeter: ");
  Serial.println(distance);
}

void publishmsg() {
  int a = distancein;
  int b = distancein * 100;
 // int a = pingavg;
 // int b = pingavg*100;
  b=b%100;
    snprintf (msg, 75, "Inches: %d.%d Status: %s", a,b, robotStatus);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(outTopic, msg);
 
}

void forwardleft() {
  robotStatus = "ForwardRight";
  digitalWrite(left, LOW);
  analogWrite(leftpwm, setspeed);
  
}

void backleft() {
  robotStatus = "BackwardRight";
  digitalWrite(left, HIGH);
  digitalWrite(leftpwm, 1023 - setspeed);
  
}

void backright() {
  robotStatus = "BackwardLeft";
  digitalWrite(right, LOW);
  digitalWrite(rightpwm, setspeed);
}

void forwardright() {
  robotStatus = "ForwardLeft";
  digitalWrite(right, HIGH);
  analogWrite(rightpwm, 1023 - setspeed);
  
}

void backward() {
  robotStatus = "Backward";
  digitalWrite(right, LOW);
  analogWrite(rightpwm, setspeed);
  digitalWrite(left, HIGH);
  analogWrite(leftpwm, 1023 - setspeed);
}

void forward() {
  robotStatus = "Forward";
  digitalWrite(left, LOW);
  analogWrite(leftpwm, setspeed);
  digitalWrite(right, HIGH);
  analogWrite(rightpwm, 1023 - setspeed);
}

void stop() {
  robotStatus = "Stop";
  digitalWrite(right, LOW);
  digitalWrite(left, LOW);
  analogWrite(rightpwm, 0);
  analogWrite(leftpwm, 0);
}

void checksonar() {

 check();
 int ping1 = (pingavg/pingcount);

if (robotStatus == "Forward") {
  if (distancein <= 10 && distancein >=0) {
  //if (ping1 <= 10 && ping1 >= 0) {
    stop();
    pingavg = 0; //reset the avgerage from ultrasonic
    pingcount = 0; //reset the counter
    check(); 
    }
  //  stop();
  }  
}
