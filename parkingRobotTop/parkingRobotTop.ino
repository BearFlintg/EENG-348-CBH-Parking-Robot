/* parkingRobotTop.ino 
 Christopher Datsikas, Bear Flintgruber, and Hope Wilson
 Created: 12-1-2014
 Last Modified: 12-1-2014
 Status: IP
 Parking Robot - Final Project for EE 348
 
 Top Arduino receiveds commands from middle Arduino via I2C/TWI
 Moves servo accordingly
 During motion sweep, IR distance data is recorded 
 and sent via I2C/TWI to middle Arduino to be sent back to PC 
 
 Components:
 MCU: Arduino Uno
 GP2Y0A21YK0F 10-80 cm IR Rangefinder
 Servo
 
 Connections
 // IR Rangefinder
 5V     <-> red
 Pin A0 <-> white
 GND    <-> black
 
 // Servo
 5V     <-> red
 Pin D9 <-> orange (D9 has PWM)
 GND    <-> black
 */

#include <Servo.h>
#define S_ADDR2 0x61  //address of the 2st slave

Servo myservo; //create myservo object to control servo

int distVal, num=5;
int servoPos=0, goalPos;

void setup()
{
  myservo.attach(9);         // servo attached in a particular pwm pin
  myservo.write(servoPos);   // servo is initially set to 90 degrees

  Wire.begin(S_ADDR2);       // Sets UNO as slave with S_ADDR2 address  
  Wire.onReceive(receiveEvent);
  
  Serial.begin(9600);        // Serial communication is set up at specified baud rate
  Serial.setTimeout(10);     // 
}

void loop()
{
 // loop should be empty because action happens only when data is 
 // received from the middle uno
}


void receiveEvent(int bytes)
{
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

// takes the average of one analog pin sampled three times at a 50ms sampling interval
int averageDist(){
  int distValA = analogRead(0);
  delay(50);
  int distValB = analogRead(0);
  delay(50);
  int distValC = analogRead(0);
  delay(50);
  return (distValA+distValB+distValC)/3;
}

