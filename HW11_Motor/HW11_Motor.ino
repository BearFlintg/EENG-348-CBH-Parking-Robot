/* HW 11_Motor
 Bear and Hope Due Oct 8th
 
 MCU connects to robot to 
 1) control power to robot via DACS, controlled by pots
 2) control the motors to go forward, then backward for 2 seconds
 3) use pots to make the robot move in a generally straight line
 
 General program form:
 Continuous change of direction on motors and update of DACs
 Interrupt function to update ADC values
 
 
 Connections:
 (Ard)   (Robot)
 Pin 2-> J4, Left wheel encoder
 Pin 3-> J5, Right wheel encoder
 Pin 6-> J6, Fin, Right motor
 Pin 7-> J7, Rin, Right motor
 Pin 8-> J8, Fin, Left motor
 Pin 9-> J9, Rin, Left motor
 
 // Bit-Bang to DAC
// CS 12 (output) -> J12 DAC (!CS) white
// DIN 11 (output) -> J11 DAC DIN gray
// SCK 10 (output) -> J10 DAC clk purple
 
 ADC4->Potentiometer 0, Right motor (DAC B)
 ADC5->Potentiometer 1, Left motor (DAC A)
 
 */
int ADC4_B, ADC5_A, direct,i,val;

// added by Hope
#define CLOCK 10
#define DOUT 11
#define CSLD 12


#define NBITS 16
int b[NBITS];


void setup(){
  Serial.begin(115200); //open serial port, set BR
  //ZxSerial.setTimout(10); //make parseInt() faster
  pinMode(6, OUTPUT); //set motor control pins to output
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  digitalWrite(6,LOW); //set all low--off
  digitalWrite(7,LOW);
  digitalWrite(8,LOW);
  digitalWrite(9,LOW);
  //added by Hope
  pinMode(CLOCK, OUTPUT);
  digitalWrite(CLOCK, LOW);
  pinMode(DOUT, OUTPUT);
  digitalWrite(DOUT, LOW);
  pinMode(CSLD, OUTPUT);
  digitalWrite(CSLD, HIGH);


  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A (Timer/Counter 1 Control Register A)  to 0 (clear)
  TCCR1B = 0;// same for TCCR1B (Timer/Counter1 Control Register B)
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 62499;// = (16*10^6 Hz) / (0.25 sec*1024 prescale) - 1 
  //(must be <65536 on Timer 1)
  TCCR1B |= (1 << WGM12); // turn on CTC mode (clear timer on compare)
  TCCR1B |= (1 << CS12);  // Set CS12 bit to 1 for 256 prescaler
  TCCR1B |= (1 << CS10);  //Set SC10 bit to 1 for 1024 prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt

  direct=1; 
}

void I2DACb(int x, int potval) {
  if (potval == 4) // potB
  { // code for Load DAC B is 0010
    b[0] = 1;
    b[1] = 0;
    b[2] = 1;
    b[3] = 0;
  }
  else // potA
  { // code for Load DAC A is 0001
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
  for (i = 0; i < 16; i++)
  {
    Serial.print(b[i]); 
  }
  Serial.println();
}

void Send2DAC(int ADC4_B, int ADC5_A){
  I2DACb(ADC4_B, 4);
  digitalWrite(CSLD, LOW);
  for (i = 0; i < NBITS; i++)
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
  for (i = 0; i < NBITS; i++)
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
    digitalWrite(6,LOW); // Forward
    digitalWrite(7,HIGH); 
    digitalWrite(8,HIGH);
    digitalWrite(9,LOW);
  }
  else if (direct==0){
    digitalWrite(6,HIGH); //Reverse
    digitalWrite(7,LOW);
    digitalWrite(8,LOW);
    digitalWrite(9,HIGH);
  }
}

ISR(TIMER1_COMPA_vect){
  //Serial.println("Timer Interrupt");
  ADC4_B=analogRead(4);
  ADC5_A=analogRead(5);
  Send2DAC(ADC4_B, ADC5_A);
  //Serial.println("End interrupt");
  //ChangeDirection(direct);

}

void loop(){
  Serial.println("Changing direction");
  ChangeDirection(direct);
  direct=(direct+1)%2; //alternates between 1 and 0
  delay(2000);
  //Send2DAC(ADC4_B, ADC5_A); 
}

