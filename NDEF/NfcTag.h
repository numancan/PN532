#ifndef NfcTag_h
#define NfcTag_h

#include <inttypes.h>
#include <NdefMessage.h>

class NfcTag
{
    public:
        NfcTag();
        NfcTag(uint8_t *uid, unsigned int uidLength);
        NfcTag(uint8_t *uid, unsigned int uidLength, std::string tagType);
        NfcTag(uint8_t *uid, unsigned int uidLength, std::string tagType, NdefMessage& ndefMessage);
        NfcTag(uint8_t *uid, unsigned int uidLength, std::string tagType, const uint8_t *ndefData, const int ndefDataLength);
        ~NfcTag(void);
        NfcTag& operator=(const NfcTag& rhs);
        uint8_t getUidLength();
        void getUid(uint8_t *uid, unsigned int uidLength);
        std::string getUidString();
        std::string getTagType();
        bool hasNdefMessage();
        NdefMessage getNdefMessage();
#ifdef NDEF_USE_SERIAL
        void print();
#endif
    private:
        uint8_t *_uid;
        unsigned int _uidLength;
        std::string _tagType; // Mifare Classic, NFC Forum Type {1,2,3,4}, Unknown
        NdefMessage* _ndefMessage;
        // TODO capacity
        // TODO isFormatted
};

#endif