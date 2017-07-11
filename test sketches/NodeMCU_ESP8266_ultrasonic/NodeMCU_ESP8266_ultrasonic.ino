/*
********************************************
14CORE ULTRASONIC DISTANCE SENSOR CODE TEST
********************************************
*/
#define TRIGGER 12 //D6
#define ECHO    14 //D5

// NodeMCU Pin D1 > TRIGGER | Pin D2 > ECHO

void setup() {
  
  Serial.begin (9600);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
}

void loop() {
  
  float duration, distance, distancein;
  digitalWrite(TRIGGER, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = (duration/2) / 29.1;
  distancein = distance * 0.39;
  Serial.print("Inch: ");
  Serial.print(distancein);
  Serial.print(" Centimeter: ");
  Serial.println(distance);
  delay(1000);
}
