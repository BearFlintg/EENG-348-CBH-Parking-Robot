/* parkingRobotMiddle.ino 
 Christopher Datsikas, Hope Wilson, Bear Flintgruber
 Created: 12-1-2014
 Last Modified: 12-1-2014
 Status: IP
 Parking Robot - Final Project for EE 348
 
 Middle Arduino via SPI receives data from Matlab which is parsed and then
 send to the other Arudinos via I2C/TWI
 
 Robot Components:
 MCU: Arduino Uno
 Wireless Module: nRF24L01
 
 Connections:
 //SPI Communication Between Arduino and Wireless Module
 (Ard)  (nRF24L01)
 Pin 9  <-> CE  (Chip Enable) 
 Pin 10 <-> CSN (Chip Select)
 Pin 11 <-> MOSI
 Pin 12 <-> MISO
 Pin 13 <-> SCK
 3V3    <-> 3V3
 GND    <-> GND
 
 //I2C/TWI Communication Between Arduinos
 Pin A4 <-> Pin A4 (SDA)
 Pin A5 <-> Pin A5 (SCL)
 GND <-> GND
 
 Software adapted from:
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

#include <Wire.h>
#define S_ADDR 0x60

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
const uint64_t pipes[2] = { 
  0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  
//

// The various roles supported by this sketch
typedef enum { 
  role_ping_out = 1, role_pong_back } 
role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { 
  "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role = role_pong_back;

int distVal, num=5;
int radioval;
int Lmotor, Rmotor, direct;

void setup(void)
{
  Wire.begin(); // Empty address defines Master (primary)

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

}

// variables to store series of bytes from Matlab/Wireless Receiver
byte xRmotor = 0; 
byte xRmotor2 = 0;
byte xLmotor = 0;
byte xLmotor2 = 0;
byte xdirect = 1;

void loop(){
  // waiting for a radio signal:
  while ( ! radio.available())
  {
  }
  // read the signal from the wireless:
  // (right motor speed, ranges from 0 to 1023)
  radio.read( &Rmotor, sizeof(int) );
  printf("right motor: ");
  printf("%d", Rmotor, "\n\r");
  
  // waiting for a radio signal:
  while ( ! radio.available())
  {
  }
  // read the signal from the wireless:
  // (left motor speed, ranges from 0 to 1023)
  radio.read( &Lmotor, sizeof(int) );
  printf("left motor: ");
  printf("%d", Lmotor, "\n\r");

  // waiting for a radio signal:
  while ( ! radio.available())
  {
  }
  //read the signal from the wireless:
  // direction, 1 if forward and 0 if backwards
  radio.read( &direct, sizeof(int) );

  Serial.print("direction: ");
  if (direct == 1)
  {
    Serial.println("forward");
  }
  else if (direct == 0)
  {
    Serial.println("backwards");
  }

  // I2C can only send data in byte-size chunks
  // but our motor speed values are 10-bit, so we have to split them up
  
  // xRmotor = least significant 8 bits of Rmotor:
  xRmotor = Rmotor % 256;
  // xRmotor2 = most significant 2 bits of Rmotor:
  // (Rmotor = xRmotor2*256 + xRmotor)
  xRmotor2 = (Rmotor - xRmotor)/256;
  // xLmotor = least significant 8 bits of Lmotor:
  xLmotor = Lmotor % 256;
  // xLmotor2 = most significant 2 bits of Lmotor:
  // (Lmotor = xLmotor2*256 + xLmotor)
  xLmotor2 = (Lmotor - xLmotor)/256;
  // direction is only one bit so it doesn't need to be split up
  xdirect = direct;
  
  // transmit all values in byte form via I2C:
  Wire.beginTransmission(S_ADDR);
  Wire.write(xRmotor);
  Wire.write(xRmotor2);
  Wire.write(xLmotor);
  Wire.write(xLmotor2);
  Wire.write(xdirect);
  Wire.endTransmission();
  Serial.println("transmitted!");
}

