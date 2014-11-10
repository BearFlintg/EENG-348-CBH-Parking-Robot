/* HW14_ctd27.ino 
 Christopher Datsikas
 Created: 11-3-2014
 Last Modified: 11-3-2014
 Status: Works
 Assignment #14 for EENG 348
 Servo and IR Ranging Module
 
 Receives angle value from Matlab
 Moves servo accordingly
 During motion sweep, IR distance data is recorded and sent over Serial to Matlab
 */

// 3 Pin GP2Y0A21YK0F 10-80 cm IR Rangefinders:
// red -> 5V
// white -> A0
// black -> GND
// SERVO
// red -> 5V
// orange -> D9 (with pwm)
// black -> ground

#include <Servo.h>

Servo myservo; //create myservo object to control servo

int distVal, num=5;
int servoPos=0, goalPos;

void setup()
{
  myservo.attach(9);         // servo attached in a particular pwm pin
  myservo.write(servoPos);   // servo is initially set to 90 degrees

  Serial.begin(9600);        // Serial communication is set up at specified baud rate
  Serial.setTimeout(10);     // 
}

void loop()
{
  if (Serial.available() > 0){       // wait for ML
    goalPos = Serial.parseInt();     // read goal angle value
    if (goalPos<0) goalPos = 0;      // protects form exceeding limits of the servo
    if(goalPos>180) goalPos = 180;
    moveTo(goalPos);
  }
}

//takes a desination value, and moves the servo to that position,
//while taking IR rangefinder sensor data every 5 degrees
void moveTo(int destination) {
  if (servoPos < destination){
    while(servoPos < destination) {
      servoPos+=5;
      myservo.write(servoPos);
      Serial.println(servoPos);
      distVal = averageDist();
      Serial.println(distVal);
    }
  }
  else if (servoPos > destination) {
    while(servoPos > destination) {
      servoPos-=5;
      myservo.write(servoPos);
      Serial.print(servoPos);
      distVal=averageDist();
      Serial.println(distVal);
    }
  }
}

// takes the average of one analog pin sampled three tapes at a 50ms sampling interval
int averageDist(){
  int distValA = analogRead(0);
  delay(50);
  int distValB = analogRead(0);
  delay(50);
  int distValC = analogRead(0);
  delay(50);
  return (distValA+distValB+distValC)/3;
}

