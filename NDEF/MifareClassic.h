#ifndef MifareClassic_h
#define MifareClassic_h

// Comment out next line to remove Mifare Classic and save memory
#define NDEF_SUPPORT_MIFARE_CLASSIC

#ifdef NDEF_SUPPORT_MIFARE_CLASSIC

#include <Due.h>
#include <PN532.h>
#include <Ndef.h>
#include <NfcTag.h>

class MifareClassic
{
    public:
        MifareClassic(PN532& nfcShield);
        ~MifareClassic();
        NfcTag read(uint8_t *uid, unsigned int uidLength);
        bool write(NdefMessage& ndefMessage, uint8_t *uid, unsigned int uidLength);
        bool formatNDEF(uint8_t * uid, unsigned int uidLength);
        bool formatMifare(uint8_t * uid, unsigned int uidLength);
    private:
        PN532* _nfcShield;
        int getBufferSize(int messageLength);
        int getNdefStartIndex(uint8_t *data);
        bool decodeTlv(uint8_t *data, int &messageLength, int &messageStartIndex);
};

#endif
#endif
