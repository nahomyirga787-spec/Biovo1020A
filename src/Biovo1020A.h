#ifndef BIOVO1020A_H
#define BIOVO1020A_H

#include <Arduino.h>

#define BIOVO_CMD_ENROLL_1   0x01
#define BIOVO_CMD_ENROLL_2   0x02
#define BIOVO_CMD_ENROLL_3   0x03
#define BIOVO_CMD_DELETE     0x04
#define BIOVO_CMD_DELETE_ALL 0x05
#define BIOVO_CMD_COUNT      0x09
#define BIOVO_CMD_SEARCH     0x0C

#define BIOVO_ACK_SUCCESS    0x00
#define BIOVO_ACK_FAIL       0x01
#define BIOVO_ACK_FULL       0x04

class Biovo1020A {
  public:
    Biovo1020A(Stream &serialPort);
    
    void begin();
    int16_t getCount(uint32_t timeout = 5000);
    bool enroll(uint16_t id, uint8_t step, uint32_t timeout = 30000);
    bool search(uint16_t &matchedID, uint8_t &permission, uint32_t timeout = 30000);
    bool deleteUser(uint16_t id, uint32_t timeout = 5000);
    bool deleteAll(uint32_t timeout = 5000);

    uint8_t getLastStatus() const { return _lastStatus; }
    const uint8_t* getLastRxPacket() const { return _rxPacket; }

  private:
    Stream &_serial;
    uint8_t _rxPacket[8];
    uint8_t _lastStatus;

    void clearBuffer();
    uint8_t calculateChecksum(const uint8_t *packet);
    bool sendCommand(uint8_t cmd, uint8_t d1, uint8_t d2, uint8_t d3, uint32_t timeout);
};

#endif