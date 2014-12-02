/* parkingRobotBottom.ino 
 Christopher Datsikas, Bear Flintgruber, and Hope Wilson
 Created: 11-29-2014
 Last Modified: 12-1-2014
 Status: IP
 Parking Robot - Final Project for EE 348
 
 Bottom Arduino sends commands to the motor driver to change direction
 and uses bit-banging to send speed command to robot DAC.
 Collects data from encoders and utilizes P control.
 Communicates to the other two Arduinos using I2C/TWI and the Wire library.
 
 Robot Components:
 MCU: Arduino Uno
 Robot DAC: LTC1661
 
 Connections:
 (Ard)   (Robot)
 Pin 2-> J4, Left wheel encoder
 Pin 3-> J5, Right wheel encoder
 
 // Motor Driver - determines direction
 Pin 6-> J6, Fin, Right motor
 Pin 7-> J7, Rin, Right motor
 Pin 8-> J8, Fin, Left motor
 Pin 9-> J9, Rin, Left motor
 
 // Bit-Bang to DAC - determines motor voltage and speed
 CS 12 (output) -> J12 DAC (!CS) white
 DIN 11 (output) -> J11 DAC DIN gray
 SCK 10 (output) -> J10 DAC clk purple
 
 //I2C/TWI Communication Between Arduinos
 Pin A4 <-> Pin A4 (SDA)
 Pin A5 <-> Pin A5 (SCL)
 GND <-> GND
 
 */

#define encoderL 2
#define encoderR 3

               // F R
#define FinR 6 // 0 0 brake
#define RinR 7 // 0 1 CW
#define FinL 8 // 1 0 CCW
#define RinL 9 // 1 1 brake

#define CLOCK 10
#define DOUT 11
#define CSLD 12

#include <Wire.h> // for I2C/TWI Arduino to Arduino Communication
#define S_ADDR 0x60  //address of the 1st slave

#define NBITS 16
int b[NBITS];

boolean debug=true; //debug mode, set to true to print out info to Serial monitor

volatile int countsL=0;
volatile int countsR=0;
boolean dirL=0; //0 = backwards, 1= forwards
boolean dirR=0;
int spL=0, spR=0; //speed of each wheel (0-1023)

unsigned long previousMillis = 0; //needed for "blinkWithoutDelay" style counter
unsigned long previousMillis2 = 0;
unsigned long currentMillis;

void setup(){
  Serial.begin(9600); //open serial port, set Baud Rate
  //ZxSerial.setTimout(10); //make parseInt() faster
  Wire.begin(S_ADDR);  // Sets UNO as slave with S_ADDR address
  Wire.onReceive(receiveEvent);
  pinMode(FinR, OUTPUT); //set motor control pins to output
  pinMode(RinR,OUTPUT);
  pinMode(FinL,OUTPUT);
  pinMode(RinL,OUTPUT);
  digitalWrite(FinR,LOW); //set all low--off
  digitalWrite(RinR,LOW);
  digitalWrite(FinL,LOW);
  digitalWrite(RinL,LOW);
  pinMode(CLOCK, OUTPUT); 
  pinMode(DOUT, OUTPUT);
  pinMode(CSLD, OUTPUT);
  digitalWrite(CLOCK, LOW);
  digitalWrite(DOUT, LOW);
  digitalWrite(CSLD, HIGH);
  pinMode(encoderL, INPUT); //encoders
  pinMode(encoderR, INPUT);
  attachInterrupt(0,encoderInterL,CHANGE); // INTO on UNO pin2
  attachInterrupt(1,encoderInterR,CHANGE); // INT1 on UNO pin3

  dirL=1; //robot initilizes to going foward at fast speed
  dirR=1;  
  spL=1000;
  spR=1000;
  send2DAC(spL,spR);
  digitalWrite(FinR,LOW); // Forward
  digitalWrite(RinR,HIGH); 
  digitalWrite(FinL,HIGH); //Forward
  digitalWrite(RinL,LOW);
}

void loop(){
  //changes direction every 2000ms
  currentMillis = millis();
  if(currentMillis - previousMillis >= 2000) {
    previousMillis = currentMillis;
    //   dirL=(dirL+1)%2; //alternates between 1 and 0
    //   dirR=(dirR+1)%2; //alternates between 1 and 0
    //   changeDirection(dirL,dirR);
    //   Serial.println("Changing direction");
  }

  // updates PID and prints the status of the encoders every 100 ms
  currentMillis = millis();
  if(currentMillis - previousMillis2 >= 400) {
    previousMillis2 = currentMillis;
    updatePID();
    if (debug==1) {
      printCounts();
    }
  }
}


// sets the DAC according to value of potentiometer
// see DAC datasheet for operation
void I2DACb(int x, int potval) {
  if (potval == 4) // potB
  { // code for Load DAC B is 1010
    b[0] = 1;
    b[1] = 0;
    b[2] = 1;
    b[3] = 0;
  }
  else // potA
  { // code for Load DAC A is 1001
    b[0] = 1;
    b[1] = 0;
    b[2] = 0;
    b[3] = 1;    
  }

  for (int i = 13; i >= 4; i--) { // MSB
    b[i] = x % 2;
    x = x / 2;
  } 
  b[14] = 0;
  b[15] = 0;
  if (debug==true) {
    Serial.print("Sent to DAC: ");
    for (int i = 0; i < 16; i++)
    {
      Serial.print(b[i]); 
    }
    Serial.println();
  }
}

// sends values of potentiometer to Robot DAC
void send2DAC(int ADC4_B, int ADC5_A){
  I2DACb(ADC4_B, 4);
  digitalWrite(CSLD, LOW);
  for (int i = 0; i < NBITS; i++)
  {
    digitalWrite(CLOCK, 0);
    delay(1);
    digitalWrite(DOUT, b[i]);
    delay(1);
    digitalWrite(CLOCK, 1);  
    delay(1); 
  }
  delay(1);
  digitalWrite(CSLD, HIGH);  
  delay(1);

  I2DACb(ADC5_A, 5);
  digitalWrite(CSLD, LOW);  
  for (int i = 0; i < NBITS; i++)
  {
    delay(1);
    digitalWrite(CLOCK, 0);
    delay(1);
    digitalWrite(DOUT, b[i]);
    delay(1);
    digitalWrite(CLOCK, 1);
  }
  delay(10);
  digitalWrite(CSLD, HIGH);  
}

//changes the direction of the robot by setting the Motor Driver 
void changeDirection(boolean directL, boolean directR){
  if(directR==1){
    digitalWrite(FinR,LOW); // Forward
    digitalWrite(RinR,HIGH); 
  }
  else if (directR==0){
    digitalWrite(FinR,HIGH); //Reverse
    digitalWrite(RinR,LOW);
  }
  if(directL==1){
    digitalWrite(FinL,HIGH); //Forward
    digitalWrite(RinL,LOW);
  }
  else if (directL==0){
    digitalWrite(FinL,LOW); //Reverse
    digitalWrite(RinL,HIGH);
  }
}

void updatePID(){
  send2DAC(spL,spR);
}

void encoderInterL() //activated when there is a change in the left encoder
{
  if (dirL == 1) {  // if the robot is going forward increment the counter
    countsL++;
  }
  else if (dirL == 0) {  //if the robot is going backwards decrement the counter
    countsL--;
  }
}

void encoderInterR() //activated when there is a change in the right encoder
{
  if (dirR == 1) { // if the robot is going forward increment the counter
    countsR++;
  }
  else if (dirR == 0) { //if the robot is going backwards decrement the counter
    countsR--;
  }
}

// prints information regarding the encoders to the Serial Monitor
void printCounts(){
  Serial.print("Right Now EncoderL is : ");
  Serial.print(digitalRead(encoderL));
  Serial.print(" and EncoderR is : ");
  Serial.println(digitalRead(encoderR));
  Serial.print("Counts L: ");
  Serial.print(countsL);
  Serial.print(" Counts R: ");
  Serial.println(countsR);
}

// variables utilized to receive data from the other UNO
byte xRmotor = 0; 
byte xRmotor2 = 0;
byte xLmotor = 0;
byte xLmotor2 = 0;
byte xdirect = 1;
// function is called when the arduino receives something over I2C/TWI
// it parses the series of bytes it receives and updates pertinant variables
// including speed and direction. Then pushes that info to the robot DAC and motor driver.
void receiveEvent(int bytes)
{
  if(Wire.available() != 0)
  {
    xRmotor = Wire.read();  //which is LSM or MSB?
    xRmotor2 = Wire.read();
    xLmotor = Wire.read();
    xLmotor2 = Wire.read();
    xdirect = Wire.read();
    if (debug==1) {
      Serial.print("Received from Master Uno: ");
      Serial.print(xRmotor2, HEX);
      Serial.print(xRmotor, HEX);
      Serial.print(' ');
      Serial.print(xLmotor2, HEX);
      Serial.print(xLmotor, HEX);
      Serial.print(' ');    
      Serial.print(xdirect);
      Serial.print("\n");
    }
    spR = xRmotor + (xRmotor2*256);
    spL = xLmotor + (xLmotor2*256);
    dirL = xdirect;
    dirR = xdirect;
    changeDirection(dirL,dirR);
    send2DAC(spL,spR);
  }
}

