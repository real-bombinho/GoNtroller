#ifndef MODBUSRELAY_H
#define MODBUSRELAY_H

#include <SoftwareSerial.h>

#define RXPin_    14 // PIN_D5  // Serial Receive pin (ModBus)
#define TXPin_     2 // PIN_D4  // Serial Transmit pin (ModBus)

#define Relay_Open  0x01
#define Relay_Close 0x00

#define mbRelay1 0x00
#define mbRelay2 0x01
#define mbRelay3 0x02
#define mbRelay4 0x03
#define mbRelayNumber 4

#define BroadCast 0x00

struct MB_Relay {
private:
  byte message[8];
  byte BusAddr;
//  uint8_t RXPin_;
//  uint8_t TXPin_;
//  SoftwareSerial *con;
  bool    isAbsent_;
  uint8_t stateResponse_;
  uint8_t state_[mbRelayNumber];
  byte    setAddress (const uint8_t adr); //set address is permanently stored
  void    writeMessage ();
  void    writeCoil(const byte rel, byte state);
  uint8_t readState(const byte rel);
public:
  MB_Relay (const uint8_t rxPin, const uint8_t txPin, const byte MB_Address);
  uint16_t msgCRC16 (byte *nData);  // writes 2 byte CRC16 after the first 6 bytes
  void     switchRelay (const byte rel, byte state);
  uint8_t  relayState(const byte rel);
  uint8_t  getAddress ();
  uint32_t hardwareVersion ();
  uint8_t  inputState ();
  bool     isPresent ();
};

#endif
