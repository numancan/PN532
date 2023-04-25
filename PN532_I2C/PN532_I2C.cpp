/**
 * @modified picospuch
 */
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "PN532_I2C.h"
#include "PN532_debug.h"

#define PN532_I2C_ADDRESS (0x48 >> 1)

PN532_I2C::PN532_I2C(uint8_t rst, uint8_t irq)
{
    _i2cIns = i2c_default;
    _sdaPin = PICO_DEFAULT_I2C_SDA_PIN;
    _sclPin = PICO_DEFAULT_I2C_SCL_PIN;
    _rstPin = rst;
    _irqPin = irq;
}

PN532_I2C::PN532_I2C(i2c_inst_t * i2c, uint8_t sda, uint8_t scl, uint8_t rst, uint8_t irq)
{
    _i2cIns = i2c;
    _sdaPin = sda;
    _sclPin = scl;
    _rstPin = rst;
    _irqPin = irq;
}

void PN532_I2C::begin()
{
    i2c_init(_i2cIns, 100 * 1000);
    gpio_set_function(_sdaPin, GPIO_FUNC_I2C);
    gpio_set_function(_sclPin, GPIO_FUNC_I2C);
    gpio_pull_up(_sdaPin);
    gpio_pull_up(_sclPin);
    // Make the I2C pins available to picotool
    // bi_decl(bi_2pins_with_func(_sdaPin, _sclPin, GPIO_FUNC_I2C));

    gpio_init(_irqPin);
    gpio_set_dir(_irqPin, GPIO_IN);

    gpio_init(_rstPin);
    gpio_set_dir(_rstPin, GPIO_OUT);

    // Reset the PN532
    gpio_put(_rstPin, 1);
    gpio_put(_rstPin, 0);
    sleep_ms(500);
    gpio_put(_rstPin, 1);
    sleep_ms(20);
}

void PN532_I2C::wakeup()
{
    sleep_ms(500); // wait for all ready to manipulate pn532
}

uint8_t PN532_I2C::write(uint8_t data)
{
    if (_txBufferIndex >= BUFFER_LENGTH) return 0;

    _txBuffer[_txBufferIndex++] = data;

    return 1;
}

uint8_t PN532_I2C::read()
{
    if (_rxBufferIndex >= _rxBufferLength) {
        DMSG("No data to read\n");
        return 0;
    }

    return _rxBuffer[_rxBufferIndex++];
}

void PN532_I2C::beginTransmission(uint8_t address) 
{
    _txAddress = address;
}

void PN532_I2C::endTransmission() 
{
    int written = i2c_write_blocking(_i2cIns, PN532_I2C_ADDRESS, _txBuffer, _txBufferIndex, false);
    
    // DMSG("I2C Number of bytes written: %d\n", written);
    
    // clear buffers, reset indexs
    memset(_txBuffer, 0, BUFFER_LENGTH);
    memset(_rxBuffer, 0, BUFFER_LENGTH);

    _txAddress = 0;
    _txBufferIndex = 0;
    _rxBufferIndex = 0;
    _rxBufferLength = 0;
}

bool PN532_I2C::requestFrom(uint8_t address, uint8_t quantity)
{   
    // reset index
    _rxBufferIndex = 0;

    _rxBufferLength = i2c_read_blocking(i2c_default, PN532_I2C_ADDRESS, _rxBuffer, quantity, false);

    // DMSG("I2C Number of bytes read: %d\n", _rxBufferLength);

    return _rxBufferLength;
}

int8_t PN532_I2C::writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen)
{
    command = header[0];
    beginTransmission(PN532_I2C_ADDRESS);

    write(PN532_PREAMBLE);
    write(PN532_STARTCODE1);
    write(PN532_STARTCODE2);

    uint8_t length = hlen + blen + 1; // length of data field: TFI + DATA
    write(length);
    write(~length + 1); // checksum of length

    write(PN532_HOSTTOPN532);
    uint8_t sum = PN532_HOSTTOPN532; // sum of TFI + DATA

    DMSG("write: ");

    for (uint8_t i = 0; i < hlen; i++)
    {
        if (write(header[i]))
        {
            sum += header[i];

            DMSG_HEX(header[i]);
        }
        else
        {
            DMSG("\nToo many data to send, I2C doesn't support such a big packet\n"); // I2C max packet: 32 uint8_ts
            return PN532_INVALID_FRAME;
        }
    }

    for (uint8_t i = 0; i < blen; i++)
    {
        if (write(body[i]))
        {
            sum += body[i];

            DMSG_HEX(body[i]);
        }
        else
        {
            DMSG("\nToo many data to send, I2C doesn't support such a big packet\n"); // I2C max packet: 32 uint8_ts
            return PN532_INVALID_FRAME;
        }
    }

    uint8_t checksum = ~sum + 1; // checksum of TFI + DATA
    write(checksum);
    write(PN532_POSTAMBLE);

    endTransmission();

    DMSG("\n");

    return readAckFrame();
}

int16_t PN532_I2C::getResponseLength(uint8_t buf[], uint8_t len, uint16_t timeout)
{
    const uint8_t PN532_NACK[] = {0, 0, 0xFF, 0xFF, 0, 0};
    uint16_t time = 0;

    do
    {
        if (requestFrom(PN532_I2C_ADDRESS, 6))
        {
            if (read() & 1)
            {          // check first uint8_t --- status
                break; // PN532 is ready
            }
        }

        sleep_ms(1);
        time++;
        if ((0 != timeout) && (time > timeout))
        {
            return -1;
        }
    } while (1);

    if (0x00 != read() || // PREAMBLE
        0x00 != read() || // STARTCODE1
        0xFF != read()    // STARTCODE2
    )
    {

        return PN532_INVALID_FRAME;
    }

    uint8_t length = read();

    // request for last respond msg again
    beginTransmission(PN532_I2C_ADDRESS);
    for (uint16_t i = 0; i < sizeof(PN532_NACK); ++i)
    {
        write(PN532_NACK[i]);
    }
    endTransmission();

    return length;
}

int16_t PN532_I2C::readResponse(uint8_t buf[], uint8_t len, uint16_t timeout)
{
    uint16_t time = 0;
    uint8_t length;

    length = getResponseLength(buf, len, timeout);

    // [RDY] 00 00 FF LEN LCS (TFI PD0 ... PDn) DCS 00
    do
    {
        if (requestFrom(PN532_I2C_ADDRESS, 6 + length + 2))
        {
            if (read() & 1)
            {          // check first uint8_t --- status
                break; // PN532 is ready
            }
        }

        sleep_ms(1);
        time++;
        if ((0 != timeout) && (time > timeout))
        {
            return -1;
        }
    } while (1);

    if (0x00 != read() || // PREAMBLE
        0x00 != read() || // STARTCODE1
        0xFF != read()    // STARTCODE2
    )
    {

        return PN532_INVALID_FRAME;
    }

    length = read();

    if (0 != (uint8_t)(length + read()))
    { // checksum of length
        return PN532_INVALID_FRAME;
    }

    uint8_t cmd = command + 1; // response command
    if (PN532_PN532TOHOST != read() || (cmd) != read())
    {
        return PN532_INVALID_FRAME;
    }

    length -= 2;
    if (length > len)
    {
        return PN532_NO_SPACE; // not enough space
    }

    DMSG("read:  ");
    DMSG_HEX(cmd);

    uint8_t sum = PN532_PN532TOHOST + cmd;
    for (uint8_t i = 0; i < length; i++)
    {
        buf[i] = read();
        sum += buf[i];

        DMSG_HEX(buf[i]);
    }
    DMSG("\n");

    uint8_t checksum = read();
    if (0 != (uint8_t)(sum + checksum))
    {
        DMSG("checksum is not ok\n");
        return PN532_INVALID_FRAME;
    }
    read(); // POSTAMBLE

    return length;
}

int8_t PN532_I2C::readAckFrame()
{
    const uint8_t PN532_ACK[] = {0, 0, 0xFF, 0, 0xFF, 0};
    uint8_t ackBuf[sizeof(PN532_ACK)];

    DMSG("wait for ack at : ");
    DMSG_INT(time_us_32());
    DMSG("\n");

    uint16_t time = 0;
    do
    {
        if (requestFrom(PN532_I2C_ADDRESS, sizeof(PN532_ACK) + 1))
        {
            if (read() & 1)
            {          // check first uint8_t --- status
                break; // PN532 is ready
            }
        }

        sleep_ms(1);
        time++;
        if (time > PN532_ACK_WAIT_TIME)
        {
            DMSG("Time out when waiting for ACK\n");
            return PN532_TIMEOUT;
        }
    } while (1);

    DMSG("ready at : ");
    DMSG_INT(time_us_32());
    DMSG("\n");

    for (uint8_t i = 0; i < sizeof(PN532_ACK); i++)
    {
        ackBuf[i] = read();
    }

    if (memcmp(ackBuf, PN532_ACK, sizeof(PN532_ACK)))
    {
        DMSG("Invalid ACK\n");
        return PN532_INVALID_ACK;
    }

    return 0;
}
