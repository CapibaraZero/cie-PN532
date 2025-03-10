/**************************************************************************/
/*!
    @file     cie_Nfc_Adafruit.cpp
    @author   Developers Italia
    @license  BSD (see License)

	Implementation of the cie_Nfc abstract class using the Adafruit_PN532 library	

	@section  HISTORY

	v1.0  - First implementation of the class
*/
/**************************************************************************/
#include "cie_Nfc_Adafruit.h"

#define PN532DEBUGPRINT Serial

/**************************************************************************/
/*!
  @brief Create with the typical breakout wiring, as described by Adafruit: https://learn.adafruit.com/adafruit-pn532-rfid-nfc/breakout-wiring
*/
/**************************************************************************/
cie_Nfc_Adafruit::cie_Nfc_Adafruit () : cie_Nfc_Adafruit(2, 5, 3, 4) {
}


/**************************************************************************/
/*!
  @brief Create with a customized wiring

  @param  clk The CLK pin number
  @param  miso The MISO pin number
  @param  mosi The MOSI pin number
  @param  ss The SS pin number
*/
/**************************************************************************/
cie_Nfc_Adafruit::cie_Nfc_Adafruit (byte clk, byte miso, byte mosi, byte ss) {
  _nfc = new Adafruit_PN532(clk, miso, mosi, ss);
  if(!_nfc->getFirmwareVersion()) {
    printf("Not connected\n");
    ESP.restart();
  }else {
    printf("Connected\n");
  };
}

/**************************************************************************/
/*!
  @brief  Initializes the PN532 board
*/
/**************************************************************************/
void cie_Nfc_Adafruit::begin() {
  _nfc->begin();
  unsigned long versiondata = _nfc->getFirmwareVersion();
  if (! versiondata) {
    PN532DEBUGPRINT.print(F("Didn't find PN53x board"));
    while (1); // halt
  }
  // Got ok data, print it out!
  PN532DEBUGPRINT.print(F("Found chip PN5")); PN532DEBUGPRINT.println((versiondata>>24) & 0b11111111, HEX); 
  PN532DEBUGPRINT.print(F("Firmware ver. ")); PN532DEBUGPRINT.print((versiondata>>16) & 0b11111111, DEC); 
  PN532DEBUGPRINT.print('.'); PN532DEBUGPRINT.println((versiondata>>8) & 0b11111111, DEC);
  _nfc->SAMConfig();
}


/**************************************************************************/
/*!
  @brief  Attempts at detecting a card (will succeed if a card is present)

  @returns  A boolean value indicating whether a card was detected or not
*/
/**************************************************************************/
bool cie_Nfc_Adafruit::detectCard() {
  return _nfc->inListPassiveTarget();
}


/**************************************************************************/
/*!
  @brief  Sends an APDU command to the CIE via the PN532 terminal and checks whether the response has a valid status word
  
  @param  command A pointer to the APDU command bytes
  @param  commandLength Length of the command
  @param  response A pointer to the buffer which will contain the response bytes
  @param  responseLength The length of the desired response

  @returns  A boolean value indicating whether the operation succeeded or not
*/
/**************************************************************************/
bool cie_Nfc_Adafruit::sendCommand(byte *command, byte commandLength, byte *response, word *responseLength) {
  bool success = _nfc->inDataExchange(command, commandLength, response, (uint16_t*)responseLength);
  return success;
}


/**************************************************************************/
/*!
    @brief  Populates a buffer with random generated bytes
	
    @param  buffer The pointer to a byte array
    @param  offset The starting offset in the buffer
    @param  length The number of random bytes to generate

*/
/**************************************************************************/
void cie_Nfc_Adafruit::generateRandomBytes(byte *buffer, const word offset, const byte length) {
  randomSeed(analogRead(0)*micros()); //Use an unconnected analog pin as the random seed
  for (word i = offset; i<offset+length; i++) {
    buffer[i] = (byte) random(256);
  }
}


/**************************************************************************/
/*!
    @brief Frees resources
*/
/**************************************************************************/
cie_Nfc_Adafruit::~cie_Nfc_Adafruit()
{
  delete _nfc;
}