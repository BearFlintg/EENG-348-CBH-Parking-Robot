/* HW11_Code.ino 
 Christopher Datsikas
 with Bear and Hope
 Created: 10-4-2014
 Last Modified: 10-4-2014
 Status: In Progress
 Assignment #11 for EENG 348
 Description:
 */
 
 #define encoderL 2
 #define encoderR 3
 
 volatile int countsL=0;
 volatile int countsR=0;
 boolean dirL=0; //0 = backwards, 1= forwards
 boolean dirR=0;
 
 void setup()
{
  pinMode(encoderL, INPUT);
  pinMode(encoderR, INPUT);
  attachInterrupt(0,encoderInterL,CHANGE); // INTO
  attachInterrupt(1,encoderInterR,CHANGE); // INT1
  Serial.begin(9600);
}

void loop()
{
  Serial.print("Right Now EncoderL is : ");
  Serial.print(digitalRead(encoderL));
  Serial.print(" and EncoderR is : ");
  Serial.println(digitalRead(encoderR));
  Serial.print("Counts L: ");
  Serial.print(countsL);
  Serial.print(" Counts R: ");
  Serial.println(countsR);
  delay(1000);
}

void encoderInterL() //activated when there is a change in the left encoder
{
  if (dirL == 1) {  // if the robot is going forward increment the counter
    countsL++;
  }
  else if (dirL == 0) {  //if 
    countsL--;
  }
  delay(5);
}

void encoderInterR() //activated when there is a change in the right encoder
{
  if (dirR == 1) {
    countsR++;
  }
  else if (dirR == 0) {
    countsR--;
  }
  delay(5);
}
