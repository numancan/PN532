#ifndef NdefRecord_h
#define NdefRecord_h

#include <Due.h>
#include <Ndef.h>
#include <string>
#include <string.h>
#include <stdint.h>

#define TNF_EMPTY 0x0
#define TNF_WELL_KNOWN 0x01
#define TNF_MIME_MEDIA 0x02
#define TNF_ABSOLUTE_URI 0x03
#define TNF_EXTERNAL_TYPE 0x04
#define TNF_UNKNOWN 0x05
#define TNF_UNCHANGED 0x06
#define TNF_RESERVED 0x07

class NdefRecord
{
    public:
        NdefRecord();
        NdefRecord(const NdefRecord& rhs);
        ~NdefRecord();
        NdefRecord& operator=(const NdefRecord& rhs);

        int getEncodedSize();
        void encode(uint8_t *data, bool firstRecord, bool lastRecord);

        unsigned int getTypeLength();
        int getPayloadLength();
        unsigned int getIdLength();

        uint8_t getTnf();
        void getType(uint8_t *type);
        void getPayload(uint8_t *payload);
        void getId(uint8_t *id);

        // convenience methods
        std::string getType();
        std::string getId();

        void setTnf(uint8_t tnf);
        void setType(const uint8_t *type, const unsigned int numBytes);
        void setPayload(const uint8_t *payload, const int numBytes);
        void setId(const uint8_t *id, const unsigned int numBytes);

#ifdef NDEF_USE_SERIAL
        void print();
#endif
    private:
        uint8_t getTnfByte(bool firstRecord, bool lastRecord);
        uint8_t _tnf; // 3 bit
        unsigned int _typeLength;
        int _payloadLength;
        unsigned int _idLength;
        uint8_t *_type;
        uint8_t *_payload;
        uint8_t *_id;
};

#endif