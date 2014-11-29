/* parkingRobotMaster.ino 
 Christopher Datsikas, Bear, and Hope
 Created: 11-29-2014
 Last Modified: 11-29-2014
 Status: IP
 Parking Robot - Final Project for EE 348
 
 
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
 */

// F R
#define FinR 6 // 0 0 brake
#define RinR 7 // 0 1 CW
#define FinL 8 // 1 0 CCW
#define RinL 9 // 1 1 brake

#define CLOCK 10
#define DOUT 11
#define CSLD 12

#define NBITS 16
int b[NBITS];

boolean dir;  //direction 1 is foward, 0 is backwards
int spL=0, spR=0; //speed of each wheel

unsigned long previousMillis = 0; //needed for "blinkWithoutDelay" style counter

void setup(){
  Serial.begin(9600); //open serial port, set BR
  //ZxSerial.setTimout(10); //make parseInt() faster
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

  dir=1;  
  spL=1000;
  spR=1000;
}

void loop(){
  
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= 2000) {
    previousMillis = currentMillis;  
    ChangeDirection(dir);
    Serial.println("Changing direction");
    dir=(dir+1)%2; //alternates between 1 and 0
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
  for (int i = 0; i < 16; i++)
  {
    Serial.print(b[i]); 
  }
  Serial.println();
}

// sends values of potentiometer to Robot DAC
void Send2DAC(int ADC4_B, int ADC5_A){
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
void ChangeDirection(int direct){
  if(direct==1){
    digitalWrite(FinR,LOW); // Forward
    digitalWrite(RinR,HIGH); 
    digitalWrite(FinL,HIGH);
    digitalWrite(RinL,LOW);
  }
  else if (direct==0){
    digitalWrite(FinR,HIGH); //Reverse
    digitalWrite(RinR,LOW);
    digitalWrite(FinL,LOW);
    digitalWrite(RinL,HIGH);
  }
}

void updatePID(){
  Send2DAC(spL,spR)
  }



