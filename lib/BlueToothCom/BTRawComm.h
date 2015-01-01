//----------------------------------------------------------------------
// NXTBeeComms.h
//
// NXTBee transmit and receive functions for byte, integer, long, boolean
// byte array and ASCII strings. Also supports transmitting packed data
// structures, checksum for corrupt message detection and a basic addressing
// scheme for broadcast/unicast.
//
// Public functions for the library
// Copyright 2012 Mark Crosbie mark@mastincrosbie.com
// http://www.mastincrosbie.com/Marks_LEGO_projects
//
// Based on code from
// Dexter Industries, 2011.
// www.dexterindustries.com
//
// Xander Soldaat
// botbench.com
//
//----------------------------------------------------------------------

#pragma platform(NXT)

#define debug writeDebugStream
#define debugLine writeDebugStreamLine

////////////////////////////////////////////////////////////////////////
// Change this value depending on the baud rate of your NXTBee
//The byte value used to set the high-speed byte rate.
////////////////////////////////////////////////////////////////////////
// const int HS_BYTE = 9600;
const int HS_BYTE = 115200;

////////////////////////////////////////////////////////////////////////
//
// Private functions used internally in the library not for public
// calling. You have been warned!
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//  Constants
////////////////////////////////////////////////////////////////////////

// The maximum message size in the Xbee is 100 bytes.
// 5 bytes for header, 95 for body
const int MAX_MSG_SIZE = 95;
const int HEADER_SIZE = 5;
const int BUFFER_SIZE = MAX_MSG_SIZE + HEADER_SIZE;
const int PACKED_HDR_SIZE = 2;

// field definitions for the header, each field is one byte
const int MSG_TYPE = 0;
const int BYTE_COUNT = 1;
const int CKSUM = 2;
const int DEST_ADDRESS = 3;
const int SRC_ADDRESS = 4;

#define START_OF_MSG HEADER_SIZE
#define BROADCAST 0

// Error codes
#define OK 0        // message was received
#define ENOHDR -1   // no valid header found in message
#define ECKSUM -2   // checksum mismatch on message

// Type field entries define the message type
const int ASCII_STRING_TYPE = 0;
const int BYTE_STRING_TYPE = 1;
const int INT_TYPE = 2;
const int LONG_TYPE = 3;
const int BYTE_TYPE = 4;
const int BOOL_TYPE = 5;
const int PACKED_TYPE = 6;
const int IRSEEKER_INFO_PACKED = 7;

////////////////////////////////////////////////////////////////////////
//  Global variables.
////////////////////////////////////////////////////////////////////////

//The buffer used to store received byte string from XBee
ubyte receiveBuffer[BUFFER_SIZE];

//The buffer used to store a byte string prior to transmission from XBee
ubyte transmitBuffer[BUFFER_SIZE];

// message header data
typedef struct {
  ubyte msgType;
  ubyte byteCount;
  ubyte checksum;
  ubyte srcAddress;
  ubyte destAddress;
} msgHdr_t;

#include "crc8.h"

////////////////////////////////////////////////////////////////////////
// The address that this NXT has chosen to listen for messages on
// By default this is BROADCAST, meaning this NXT will receive all
// messages broadcast. If you want to bind this NXT to a specific
// address from 1..255 then call setMyAddress()
////////////////////////////////////////////////////////////////////////
int myAddress = BROADCAST;

////////////////////////////////////////////////////////////////////////
// transmitBytes
//
// Transmit a byte string using XBee. This methods sends a buffer of
// data to a given address. This is a low-level function called by the
// other transmit functions, so you don't need to  this yourself.
//
// Parameters:
// address: address to send message to, or 0 for broadcast
// type: the message type code
// bufferSize: The size of the buffer, excluding header, to send.
//
// Returns: The count of bytes sent.
////////////////////////////////////////////////////////////////////////
int transmitBytes(ubyte address, ubyte type, int bufferSize)
{
  if(bufferSize > MAX_MSG_SIZE) {
    bufferSize = MAX_MSG_SIZE;
  }
  transmitBuffer[BYTE_COUNT] = bufferSize;
  transmitBuffer[MSG_TYPE] = type;
  transmitBuffer[DEST_ADDRESS] = address;
  transmitBuffer[SRC_ADDRESS] = myAddress;

  //calcTransmitChecksum();

  transmitBuffer[CKSUM] = calcCRC8(true);

  //int retVal = nxtWriteRawHS(transmitBuffer[0], bufferSize+HEADER_SIZE);
  int retVal = nxtWriteRawBluetooth(transmitBuffer, bufferSize+HEADER_SIZE);
  return retVal;
}

///////////////////////////


////////////////////////////////////////////////////////////////////////
//
// Public functions start here
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//  setupHighSpeedLink
//  Purpose:  Sets the XBee port S4 to the high speed mode.
////////////////////////////////////////////////////////////////////////
void setupHighSpeedLink() {
	// Initialize port S4 to "high speed" mode.
	//nxtEnableHSPort();
	//nxtSetHSBaudRate(HS_BYTE);
  //nxtHS_Mode = hsRawMode;

  setBluetoothRawDataMode();
}

////////////////////////////////////////////////////////////////////////
//  Function:  getString
//  Purpose:  Converts the receiveBuffer contents to a string
//  Parameters:
//              string& bufferString (reference).
////////////////////////////////////////////////////////////////////////
void getString(string& bufferString) {

  int len;

  len = receiveBuffer[BYTE_COUNT];
  if(len > 20)
    len = 20;

   string sTemp = "";
   bufferString = "";
   for (int ndx = START_OF_MSG; ndx < START_OF_MSG+len; ++ndx)
   {
 	   StringFormat(sTemp, "%c", receiveBuffer[ndx]);
     bufferString += sTemp;
   }
}

////////////////////////////////////////////////////////////////////////
//  Function:  getBytes
//  Purpose:  Converts the receiveBuffer contents to a byte array
//  Parameters:
//              b: byte array (reference). Assumes this is large enough
//  Returns: the number of bytes copied into the array
////////////////////////////////////////////////////////////////////////
byte getBytes(const byte &b) {

  int len;

  len = receiveBuffer[BYTE_COUNT];

  //memcpy(b, receiveBuffer[START_OF_MSG], len);

  return len;
}

////////////////////////////////////////////////////////////////////////
//  Function:  Receive
//
//  Purpose:  Poll XBee and return the formatted message in the
//            receiveBuffer
//  Parameters:
//  hdr: (reference) the header details of the message received.
//  Returns: The count of bytes in the message body, excluding the header.
//  Returns zero if no valid message read
//  -1 if a CRC failure or other corruption error was detected
//  Count is the message body size, excluding the header
//
////////////////////////////////////////////////////////////////////////
int Receive(msgHdr_t &hdr) {

	// Read bytes into the temporary buffer.
  //int bytesRead = nxtReadRawHS(receiveBuffer[0], sizeof(receiveBuffer));
  //int bytesRead = nxtReadRawBluetooth(receiveBuffer[0], sizeof(receiveBuffer));

int bytesRead = 0;
   //If we have read anything, process it.
	if (bytesRead > 0)	{

	  debugLine("Read message of bytes %d", bytesRead);

		// first we check to see if we have enough bytes to form a valid message
		// we must at least have a header!
		if(bytesRead < HEADER_SIZE) {
		  debugLine("Message size too small");
			return ENOHDR; // we've no valid message found
		}

		// this could still be a corrupt message!
		hdr.msgType = receiveBuffer[MSG_TYPE];
		hdr.byteCount = receiveBuffer[BYTE_COUNT];
		hdr.checksum = receiveBuffer[CKSUM];
		hdr.srcAddress = receiveBuffer[SRC_ADDRESS];
		hdr.destAddress = receiveBuffer[DEST_ADDRESS];

		debug("type: %d ", hdr.msgType);
		debug("byteCount: %d ", hdr.byteCount);
		debug("checksum: %d ", hdr.checksum);
		debug("src: %d ", hdr.srcAddress);
		debugLine("dest: %d ", hdr.destAddress);

		// eliminate garbage byte count sizes
		if( (hdr.byteCount < 0) || (hdr.byteCount > BUFFER_SIZE) ) {
		  return ENOHDR;
		}

		for( int i=0; i < hdr.byteCount; i++) {
		  debug("%d ", receiveBuffer[i]);
	  }
		debugLine("--");

		// make sure we have a meaningful message size
		int len=hdr.byteCount+START_OF_MSG;
		if( (hdr.byteCount <= 0) || (hdr.byteCount > BUFFER_SIZE) )
		  return ENOHDR;

		debugLine("--------------");

		// is this message addressed to me, or to broadcast, or if I have
    // set my address to be BROADCAST, then accept the message
    if( (hdr.destAddress == BROADCAST) |
        (hdr.destAddress == (byte)(myAddress & 0xFF)) |
        (myAddress == BROADCAST) ) {

			// ok, we have a header, lets check the checksum
			//if(calcReceiveChecksum() != hdr.checksum) {
      ubyte crc = calcCRC8(false);
      if(crc != hdr.checksum) {
	  		debugLine("Bad checksum");
	  		debug("crc = %d and ", crc);
	  		debugLine("hdr.checksum = %d", hdr.checksum);
				return ECKSUM;
			}
			return hdr.byteCount;
	  }
	} else {
		return ENOHDR;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////
// getNextType
// Returns the type of the next value in the transmit buffer
// so you know what function to call to retrieve the value
// This function is only used when parsing a buffer containing a
// packed data format.
//
// Parameters:
// currPtr: (in/out) current location in the buffer to read from
//
// Returns: the type of the next packed message, or -1 if no
// more messages are found
////////////////////////////////////////////////////////////////////////
byte getNextType(int &currPtr) {

  if(currPtr >= (START_OF_MSG + receiveBuffer[BYTE_COUNT]))
    return -1;

   return receiveBuffer[currPtr++];
}

////////////////////////////////////////////////////////////////////////
// unpackValue
// Unpack the next value from packed message in the buffer
//
// Returns the value unpacked as a long.
//
// Assumes that you know the type of the packed message
// is not a string or byte array, if it is you'll get
// garbage returned.
//
// Parameters:
// currPtr: (in/out) the current point in the array to unpack from
//
////////////////////////////////////////////////////////////////////////
long unpackValue(int &currPtr) {

  long lval;
  byte byteCount = receiveBuffer[currPtr];
  currPtr++;

  // Reassemble the messages, depending on their expected size.
  if (byteCount == 4) {
    lval = (long)receiveBuffer[currPtr+3] + ((long)receiveBuffer[currPtr+2] << 8) + ((long)receiveBuffer[currPtr+1] << 16) + ((long)receiveBuffer[currPtr] << 24);
    currPtr += 4;
    return lval;
  }

  if (byteCount == 3) {
    lval = (long)receiveBuffer[currPtr + 2] + ((long)receiveBuffer[currPtr + 1] << 8) + ((long)receiveBuffer[currPtr] << 16);
    currPtr += 3;
    return lval;
  }

  if (byteCount == 2) {
    lval = (long)receiveBuffer[currPtr + 1] + ((long)receiveBuffer[currPtr] << 8);
    currPtr += 2;
    return lval;
  }

  if (byteCount == 1) {
    lval = (long)receiveBuffer[currPtr];
    currPtr++;
    return lval;
  }

  return 0;

}

////////////////////////////////////////////////////////////////////////
// unpackString
//
// Unpack a string from the packed buffer at the current position and
// return in the reference parameter s
// Only call this if you know that the next data type in the packed
// buffer is a string.
//
// Parameters:
// currPtr: (in/out) current location in the buffer to read from
// s: (out) string unpacked from the receive buffer
////////////////////////////////////////////////////////////////////////
void unpackString(int &currPtr, string &s) {

  int len;

  len = receiveBuffer[currPtr];
  currPtr++;
  if(len > 20)
    len = 20;

   string sTemp = "";
   s = "";
   for (int ndx = currPtr; ndx < currPtr+len; ++ndx)
   {
 	   StringFormat(sTemp, "%c", receiveBuffer[ndx]);
     s += sTemp;
   }

   currPtr += len;

}

////////////////////////////////////////////////////////////////////////
// unpackBytes
//
// Unpack a byte array from the packed buffer at the current position
// and return in the reference parameter b
// Only call this function if you know that the next data type is a
// packed byte array
//
// Parameters:
// currPtr: (in/out) current location in the buffer to read from
// b (out): byte array unpacked from the buffer. Must be allocated by
//          the caller with sufficieint space.
////////////////////////////////////////////////////////////////////////
int unpackBytes(int &currPtr, byte &b) {

  int len;

  len = receiveBuffer[currPtr];
  currPtr++;

  //memcpy(b, receiveBuffer[currPtr], len);

  currPtr += len;
  return len;
}

////////////////////////////////////////////////////////////////////////
// getValue
//
// Returns a value from the message just received as a long
// this allows returning anything from a single byte up to
// a 4 byte long as a reply
//
// If the count of bytes in the message is not 1..4 then 0
// is returned. Don't call this function if you are expecting
// a string or a byte array.
//
// This function will not work on packed buffers.
//
////////////////////////////////////////////////////////////////////////
long getValue() {

  byte byteCount = receiveBuffer[BYTE_COUNT];

  // Reassemble the messages, depending on their expected size.
  if (byteCount == 4)
    return (long)receiveBuffer[START_OF_MSG+3] + ((long)receiveBuffer[START_OF_MSG+2] << 8) + ((long)receiveBuffer[START_OF_MSG+1] << 16) + ((long)receiveBuffer[START_OF_MSG] << 24);
  else if (byteCount == 3)
    return (long)receiveBuffer[START_OF_MSG + 2] + ((long)receiveBuffer[START_OF_MSG + 1] << 8) + ((long)receiveBuffer[START_OF_MSG] << 16);
  else if (byteCount == 2)
    return (long)receiveBuffer[START_OF_MSG + 1] + ((long)receiveBuffer[START_OF_MSG] << 8);
  else if (byteCount == 1)
    return (long)receiveBuffer[START_OF_MSG];

  return 0;
}



////////////////////////////////////////////////////////////////////////
//  transmitInt
//
// Purpose:  Transmit an int (2 bytes) using XBee. Send value immediately.
//
// Parameters:
// address: address to send message to, or 0 for broadcast
// value - The 2 byte integer value to transmit.
//
// Returns: The count of bytes sent.
////////////////////////////////////////////////////////////////////////
int transmitInt(ubyte address, int value)
{
  transmitBuffer[START_OF_MSG] = (value >> 8) & 0xFF;
  transmitBuffer[START_OF_MSG+1] = (value >> 0) & 0xFF;

  int retVal = transmitBytes(address, INT_TYPE, 2);
  return retVal;
}

////////////////////////////////////////////////////////////////////////
//  transmitByte
//
// Purpose:  Transmit a byte using XBee. Sends value immediately.
//
// Parameters:
// address: address to send message to, or 0 for broadcast
// value - The byte value to transmit. Signed byte value
//
// Returns: The count of bytes sent.
////////////////////////////////////////////////////////////////////////
int transmitByte(ubyte address, byte value)
{
  transmitBuffer[START_OF_MSG] = value;

  int retVal = transmitBytes(address, BYTE_TYPE, 1);
  return retVal;
}

////////////////////////////////////////////////////////////////////////
//  transmitBool
//
//  Purpose:  Transmit a boolean using XBee. Sends value immediately.
//
//  Parameters:
//  address: address to send message to, or 0 for broadcast
//  value - The value to transmit. True or false
//
//  Returns: The count of bytes sent.
////////////////////////////////////////////////////////////////////////
int transmitBool(ubyte address, bool value)
{
  transmitBuffer[START_OF_MSG] = (value?1:0);

  int retVal = transmitBytes(address, BOOL_TYPE, 1);
  return retVal;
}


////////////////////////////////////////////////////////////////////////
//  transmitLong
//
//  Purpose:  Transmit a long (4 bytes) using XBee. Sends value immediately
//
//  Parameters:
//  address: address to send message to, or 0 for broadcast
//  value:   The 4 byte long value to transmit.
//
//  Returns: The count of bytes sent.
////////////////////////////////////////////////////////////////////////
int transmitLong(ubyte address, long value)
{
  transmitBuffer[START_OF_MSG] = (value >> 24) & 0xFF;
  transmitBuffer[START_OF_MSG+1] = (value >> 16) & 0xFF;
  transmitBuffer[START_OF_MSG+2] = (value >>  8) & 0xFF;
  transmitBuffer[START_OF_MSG+3] = (value >>  0) & 0xFF;

  int retVal = transmitBytes(address, LONG_TYPE, 4);
  return retVal;
}


////////////////////////////////////////////////////////////////////////
//  transmitASCII
//
//  Purpose:  Transmit an ASCII string using XBee. Sends string immediately.
//
//  Parameters:
//  address: address to send message to, or 0 for broadcast
//  asciiString: The ASCII string to transmit.
//
//  Returns: The count of bytes sent.
////////////////////////////////////////////////////////////////////////
int transmitASCII(ubyte address, string asciiString)
{
  int len;

  len = strlen(asciiString);
  if(len > MAX_MSG_SIZE) {
    len = MAX_MSG_SIZE;
  }
  //Copy the ASCII string to the buffer.
  //memcpy(transmitBuffer[START_OF_MSG], asciiString, len);
	memcpy(transmitBuffer, asciiString, len);

  int retVal = transmitBytes(address, ASCII_STRING_TYPE, len);
  return retVal - 1;
}

////////////////////////////////////////////////////////////////////////
//  transmitBytearray
//
//  Purpose:  Transmit a byte array using XBee. Sends bytes immediately.
//
//  Parameters:
//  address: address to send message to, or 0 for broadcast
//  array: The byte array to transmit.
//  arrayLen: the number of byte elements in the array
//  Returns: The count of bytes sent.
////////////////////////////////////////////////////////////////////////
int transmitBytearray(ubyte address, const byte &array, int arrayLen) {

  // make sure we have room for arrayLen more bytes!
  if((arrayLen + START_OF_MSG) < MAX_MSG_SIZE) {

    //Copy the byte array to the buffer.
    //memcpy(transmitBuffer[START_OF_MSG], array, arrayLen);

	  return transmitBytes(address, BYTE_STRING_TYPE, arrayLen);
  }

	return 0;
}

////////////////////////////////////////////////////////////////////////
// initialisePacking
//
// Initialise the transmitBuffer for sending packed values
//
// Clears the buffer, and places a special packing header into the
// array. The CRC8 value will be computed across the entire packed
// buffer, not each individual message, when the buffer is sent
//
// This function must be called before any pack* function
//
// Parameters:
//	  currPtr: the current pointer into the transmitBuffer where
//    the first packed message can be stored
//
////////////////////////////////////////////////////////////////////////
void initialisePacking(void) {

	//memset(transmitBuffer[0], 0, BUFFER_SIZE);
	transmitBuffer[MSG_TYPE] = PACKED_TYPE;
	transmitBuffer[BYTE_COUNT] = 0;
}

void initialiseIRSeekerPacked(void) {
	transmitBuffer[MSG_TYPE] = 	IRSEEKER_INFO_PACKED;
	transmitBuffer[BYTE_COUNT] = 0;
}

////////////////////////////////////////////////////////////////////////
// transmitPackedMessage
//
// Transmit a packed message buffer, computing the CRC8 before sending
// and setting the Byte count
//
// Parameters:
//  address: address to send message to, or 0 for broadcast
//
// Returns: number of bytes packed into the buffer, or 0 if none
//
////////////////////////////////////////////////////////////////////////
int transmitPackedMessage(ubyte address) {

  transmitBuffer[DEST_ADDRESS] = address;
  transmitBuffer[SRC_ADDRESS] = myAddress;
  transmitBuffer[CKSUM] = calcCRC8(true);

  int len = transmitBuffer[BYTE_COUNT];
  int retVal = nxtWriteRawBluetooth(transmitBuffer, len+HEADER_SIZE);
  return retVal;
}

////////////////////////////////////////////////////////////////////////
// packByte
//
// Pack a byte  into the sending message array if there is space
//
// Parameters:
//    value: the byte  value to pack
//
// Returns: number of bytes packed into the buffer, or 0 if none
//
////////////////////////////////////////////////////////////////////////
int packByte(byte value) {
  int currCount, i;

  // get the current index into the transmit buffer for the next
  // data element
  currCount = transmitBuffer[BYTE_COUNT];

  // make sure we have room for 1 more byte and a 2-byte header!
  if((currCount + 1 + PACKED_HDR_SIZE) < MAX_MSG_SIZE) {
    i = START_OF_MSG + currCount;
    transmitBuffer[i++] = BYTE_TYPE;
    transmitBuffer[i++] = 1;
	  transmitBuffer[i] = value;

	  // update the total count of bytes in the buffer by 1 + 2
	  transmitBuffer[BYTE_COUNT] = currCount + 1 + 2;

	  return 1;
  }

	return 0;
}

////////////////////////////////////////////////////////////////////////
// packBool
// Pack a boolean into the sending message array if there is space
// Parameters:
//    value: the boolean value to pack
// Returns: number of bytes packed into the buffer, or 0 if none
//
int packBool(bool value) {
  int currCount, i;

  // get the current index into the transmit buffer for the next
  // data element
  currCount = transmitBuffer[BYTE_COUNT];

  // make sure we have room for 1 more byte and a 2-byte header!
  if((currCount + 1 + PACKED_HDR_SIZE) < MAX_MSG_SIZE) {
    i = START_OF_MSG + currCount;
    transmitBuffer[i++] = BOOL_TYPE;
    transmitBuffer[i++] = 1;
	  transmitBuffer[i] = value?1:0;

	  // update the total count of bytes in the buffer by 1 + 2
	  transmitBuffer[BYTE_COUNT] = currCount + 1 + 2;

	  return 1;
  }

	return 0;
}


////////////////////////////////////////////////////////////////////////
// packInt
//
// Pack a 2 byte integer into the sending message array if there is space
//
// Parameters:
//    value: the 2 byte integer value to pack
// Returns: number of bytes packed into the buffer, or 0 if none
// Updates the BYTE_COUNT in the transmitBuffer in place
//
////////////////////////////////////////////////////////////////////////
int packInt(int value) {

  int currCount, i;

  // get the current index into the transmit buffer for the next
  // data element
  currCount = transmitBuffer[BYTE_COUNT];

  // make sure we have room for 2 more bytes and a 2-byte header!
  if((currCount + 2 + PACKED_HDR_SIZE) < MAX_MSG_SIZE) {
    i = START_OF_MSG + currCount;
    transmitBuffer[i++] = INT_TYPE;
    transmitBuffer[i++] = 2;
	  transmitBuffer[i++] = (value >> 8) & 0xFF;
	  transmitBuffer[i] = (value >> 0) & 0xFF;

	  // update the total count of bytes in the buffer by 2 + 2
	  transmitBuffer[BYTE_COUNT] = currCount + 4;

	  return 2;
  }
	return 0;
}

////////////////////////////////////////////////////////////////////////
// packLong
//
// Pack a 4 byte long into the sending message array if there is space
// Parameters:
//    value: the 4 byte long value to pack
// Returns: number of bytes packed into the buffer, or 0 if none
//
////////////////////////////////////////////////////////////////////////
int packLong(long value) {

  int currCount, i;

  // get the current index into the transmit buffer for the next
  // data element
  currCount = transmitBuffer[BYTE_COUNT];

  // make sure we have room for 4 more bytes and a 2-byte header!
  if((currCount + 4 + PACKED_HDR_SIZE) < MAX_MSG_SIZE) {
    i = START_OF_MSG + currCount;

    transmitBuffer[i++] = LONG_TYPE;
    transmitBuffer[i++] = 4;
    transmitBuffer[i++] = (value >> 24) & 0xFF;
    transmitBuffer[i++] = (value >> 16) & 0xFF;
    transmitBuffer[i++] = (value >>  8) & 0xFF;
    transmitBuffer[i] = (value >>  0) & 0xFF;

	  // update the total count of bytes in the buffer by 4 + 2
	  transmitBuffer[BYTE_COUNT] = currCount + 4 + 2;

	  return 4;
  }
	return 0;
}

////////////////////////////////////////////////////////////////////////
// packString
//
// Pack a string into the sending message array if there is space
//
// Parameters:
//    asciiString: a string value to pack
//
// Returns: number of bytes packed into the buffer, or 0 if none
//
////////////////////////////////////////////////////////////////////////
int packString(string asciiString) {

  int currCount, len, i;

  // get the current index into the transmit buffer for the next
  // data element
  currCount = transmitBuffer[BYTE_COUNT];
  len = strlen(asciiString);

  // make sure we have room for 4 more bytes and a 2-byte header!
  if((currCount + len + PACKED_HDR_SIZE) < MAX_MSG_SIZE) {
    i = START_OF_MSG + currCount;

    transmitBuffer[i++] = ASCII_STRING_TYPE;
    transmitBuffer[i++] = len;

    //Copy the ASCII string to the buffer.
    //memcpy(transmitBuffer[i], asciiString, len);

    // update the total count of bytes in the buffer by len + 2
	  transmitBuffer[BYTE_COUNT] = currCount + len + 2;

	  return len;
  }

	return 0;
}


////////////////////////////////////////////////////////////////////////
// packBytes
//
// Pack a byte array into the sending message array if there is space
//
// Parameters:
//    array: the byte array value to pack
//	  arrayLen: the number of bytes to pack
//
// Returns: number of bytes packed into the buffer, or 0 if none
//
////////////////////////////////////////////////////////////////////////
int packBytes(const byte &array, int arrayLen) {
  int currCount, i;

  // get the current index into the transmit buffer for the next
  // data element
  currCount = transmitBuffer[BYTE_COUNT];

  // make sure we have room for arrayLen more bytes and a 2-byte header!
  if((currCount + arrayLen + PACKED_HDR_SIZE) < MAX_MSG_SIZE) {
    i = START_OF_MSG + currCount;

    transmitBuffer[i++] = BYTE_STRING_TYPE;
    transmitBuffer[i++] = arrayLen;

    //Copy the byte array to the buffer.
    //memcpy(transmitBuffer[i], array, arrayLen);

    // update the total count of bytes in the buffer by len + 2
	  transmitBuffer[BYTE_COUNT] = currCount + arrayLen + 2;

	  return arrayLen;
  }

	return 0;
}

////////////////////////////////////////////////////////////////////////
// setMyAddress
//
// Sets the address for this NXT. The NXT will listen for only those
// messages targeted at this address. If the message does not match
// this address then it is discarded.
//
// 0 (default) is the broadcast address. Setting an address of 0 means
// that this NXT will receive all messages broadcast.
//
// Parameters:
//    address: the address to associated with this NXT
//
////////////////////////////////////////////////////////////////////////
void setMyAddress(ubyte address) {
  myAddress = address;
}
