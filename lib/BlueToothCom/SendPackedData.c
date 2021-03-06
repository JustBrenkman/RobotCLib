//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// SendPackedData.c
//
// Test packing multiple data formats into a single message and
// sending them to a receipient (either another NXT or a PC/Mac)
//
// The packed data values change each iteration, so you should
// see changing values on the receiver.
//
// Run this on a NXT with a NXTBee attached to port 4.
//
// To receive data on another NXT run the Receiver.c program
// on the NXT. To receive data on your PC/Mac run the Reader.pde
// Processing sketch. You can run two receivers concurrently,
// they should both get the same values sent by this program.
//
// Copyright 2012 Mark Crosbie mark@mastincrosbie.com
// http://www.mastincrosbie.com/Marks_LEGO_projects
//

#include "NXTBeeComms.h"

// default broadcast this message
#define ADDR BROADCAST

task main() {

  byte array[5] = {2,4,6,8,10};
  long lvalue = 123456789;
  byte b = 123;
  eraseDisplay();
  bNxtLCDStatusDisplay = false;
  setupHighSpeedLink();

  nxtDisplayString(1, "NXTBee Pack");

  while(nNxtButtonPressed != kEnterButton) {

    // required each time through the loop to prepare the
    // buffer for packing data
    initialisePacking();

    packByte(b);
    nxtScrollText("Byte: %d", b++);

    packInt(12345);
    nxtScrollText("Int: 12345");

    packLong(lvalue);
    nxtScrollText("Long: %d", lvalue++);

    packString("Hello");
    packBool(true);
    packBytes(array, 5);

    // send the packed message to the recipient address
    transmitPackedMessage(ADDR);

    wait10Msec(50);

  }
}
