/**
 * @modified picospuch
 */

#ifndef __PN532_I2C_H__
#define __PN532_I2C_H__

#include "PN532Interface.h"

#define BUFFER_LENGTH 64

class PN532_I2C : public PN532Interface
{
public:
    PN532_I2C(uint8_t, uint8_t);
    PN532_I2C(i2c_inst_t*, uint8_t, uint8_t, uint8_t, uint8_t);

    void begin();
    void wakeup();
    void beginTransmission(uint8_t);
    void endTransmission();
    bool requestFrom(uint8_t, uint8_t);
    virtual int8_t writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body = 0, uint8_t blen = 0);
    int16_t readResponse(uint8_t buf[], uint8_t len, uint16_t timeout);

private:
    uint8_t command = 0;
    i2c_inst_t *_i2cIns;
    uint8_t _sdaPin;
    uint8_t _sclPin;
    uint8_t _rstPin;
    uint8_t _irqPin;

    uint8_t _txAddress = 0;
    uint8_t _txBuffer[BUFFER_LENGTH];
    uint8_t _txBufferIndex = 0;

    uint8_t _rxBuffer[BUFFER_LENGTH];
    uint8_t _rxBufferIndex = 0;
    uint8_t _rxBufferLength = 0;

    int8_t  readAckFrame();
    int16_t getResponseLength(uint8_t buf[], uint8_t len, uint16_t timeout);

    uint8_t write(uint8_t data);
    uint8_t read();
};


#endif
