/* parkingRobotReceiving.ino 
 Christopher Datsikas, Hope Wilson, Bear Flintgruber
 Created: 12-1-2014
 Last Modified: 12-1-2014
 Status: IP
 Parking Robot - Final Project for EE 348
 
 Receiving Arduino is connected to the computer via Serial connection
 and sends data to middle Arduino wirelessly
 
 Robot Components:
 MCU: Arduino Uno
 Wireless Module: nRF24L01
 
 Connections: todo: check these pinouts
 //SPI Communication Between Arduino and Wireless Module
 (Ard)  (nRF24L01)
 Pin 9  <-> CE  (Chip Enable) 
 Pin 10 <-> CSN (Chip Select)
 Pin 11 <-> MOSI
 Pin 12 <-> MISO
 Pin 13 <-> SCK
 3V3    <-> 3V3
 GND    <-> GND
 
 
 Software adapted from:
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */



int ADC4_B, ADC5_A, direct,i,val;
int Lmotor, Rmotor;

#define CLOCK 10
#define DOUT 11
#define CSLD 12

#define NBITS 16
int b[NBITS];

// RF_IR_ML_0
// wireless IR range communication to Matlab

// Arduino attached to servo & IR range sensor, but not Matlab.

/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example for Getting Started with nRF24L01+ radios. 
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two 
 * different nodes.  Put one of the nodes into 'transmit' mode by connecting 
 * with the serial monitor and sending a 'T'.  The ping node sends the current 
 * time to the pong node, which responds by sending the value back.  The ping 
 * node can then see how long the whole cycle took.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
 
/**
 * @file printf.h
 *
 * Setup necessary to direct stdout to the Arduino Serial library, which
 * enables 'printf'
 */

#ifndef __PRINTF_H__
#define __PRINTF_H__

#ifdef ARDUINO

int serial_putc( char c, FILE * ) 
{
  Serial.write( c );

  return c;
} 

void printf_begin(void)
{
  fdevopen( &serial_putc, 0 );
}

#else
#error This example is only for use on Arduino.
#endif // ARDUINO

#endif // __PRINTF_H__

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 

RF24 radio(9,10);

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  
//

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role = role_pong_back;

int distVal, num=5;
int servoPos=0, goalPos;
int radioval;

void setup(void)
{
  //
  // Print preamble
  //
 
  Serial.begin(57600);
  printf_begin();
  printf("\n\rRF24/examples/GettingStarted/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);
  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");

  //
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  //radio.setPayloadSize(8);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

  //if ( role == role_ping_out )
  {
   // radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
  }
  //else
  {
    //radio.openWritingPipe(pipes[1]);
    //radio.openReadingPipe(1,pipes[0]);
  }

  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
  
  //ZxSerial.setTimout(10); //make parseInt() faster
  
 direct=1; 
}

void loop(){
  int gotitR = 0;
  int gotitL = 0;
  int gotitd = 0;
  radio.stopListening();
  while ( ! Serial.available())
  {
  }
  Rmotor = Serial.parseInt();
  printf("right motor: ");
  printf("%d", Rmotor, "\n\r");
  while (gotitR == 0)
  {
    bool ok = radio.write( &Rmotor, sizeof(int) );
    if (ok)
    {
      Serial.println("ok...");
      gotitR = 1;
    }
    else
    {
      printf("failed.\n\r");
    }
  }
  radio.startListening();
  radio.stopListening();
  while ( ! Serial.available())
  {
  }
  Lmotor = Serial.parseInt();
  printf("left motor: ");
  printf("%d", Lmotor, "\n\r");
  
  while (gotitL == 0)
  {
    bool okay = radio.write( &Lmotor, sizeof(int) );
    if (okay)
    {
      Serial.println("okay...");
      gotitL = 1;
    }
    else
    {
      printf("failed.\n\r");
    }
  }
  radio.startListening();
  radio.stopListening();
 while ( ! Serial.available())
  {
  }
  direct = Serial.parseInt();
  printf("direction: ");
  if (direct == 1)
  {
    Serial.println("forward");
  }
  else if (direct == 0)
  {
    Serial.println("backwards");
  }
  
  while (gotitd == 0)
  {
     bool okayy = radio.write( &direct, sizeof(int) );
    if (okayy)
    {
      Serial.println("okayy...");
      gotitd = 1;
    }
    else
    {
      printf("failed.\n\r");
    }
  }
  
  //Send2DAC(ADC4_B, ADC5_A); 
}
