
//L298 Driver board, 2 DC motors, 2 PWM pins used

/* By LeRoy Miller, 2017
 *  This is a simple test, will spin the left motor forward
 *  for about 3 seconds, spin the right motor forward for
 *  about 3 seconds, spin the left wheel backward for 3 seconds
 *  and finally spin the right wheel backward for 3 seconds.
 *  
 *  PWM is not tested in this sketch but is used to drive the motors
 *  at full speed.
 *  
 *  Feel free to use and modify as needed.
 */

int left = 2; //D4
int right = 0; //D3
int leftpwm = 4; //D2
int rightpwm = 5; //D1

int setspeed = 500; //max 1023

void setup() {
  // put your setup code here, to run once:

pinMode(left, OUTPUT);
pinMode(right, OUTPUT);
pinMode(leftpwm, OUTPUT);
pinMode(rightpwm, OUTPUT);

stop();
delay(3000);
backright();
delay(3000);
stop();


}

void loop() {
  // put your main code here, to run repeatedly:


}

void forwardleft() {
  digitalWrite(left, LOW);
  analogWrite(leftpwm, setspeed);
  
}

void backleft() {
  digitalWrite(left, HIGH);
  digitalWrite(leftpwm, 1023 - setspeed);
  
}

void backright() {
  digitalWrite(right, LOW);
  digitalWrite(rightpwm, setspeed);
}

void forwardright() {
  digitalWrite(right, HIGH);
  analogWrite(rightpwm, 1023 - setspeed);
  
}

void backward() {
  digitalWrite(right, LOW);
  analogWrite(rightpwm, setspeed);
  digitalWrite(left, HIGH);
  analogWrite(leftpwm, 1023 - setspeed);
}

void forward() {
  digitalWrite(left, LOW);
  analogWrite(leftpwm, setspeed);
  digitalWrite(right, HIGH);
  analogWrite(rightpwm, 1023 - setspeed);
}

void stop() {
  digitalWrite(right, LOW);
  digitalWrite(left, LOW);
  analogWrite(rightpwm, 0);
  analogWrite(leftpwm, 0);
}

