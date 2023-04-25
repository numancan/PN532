add_library(PN532 INTERFACE)
target_sources(PN532 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/NDEF/MifareClassic.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NDEF/MifareUltralight.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NDEF/Ndef.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NDEF/NdefMessage.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NDEF/NdefRecord.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NDEF/NfcAdapter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/NDEF/NfcTag.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PN532/emulatetag.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PN532/llcp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PN532/PN532.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PN532/snep.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PN532_I2C/PN532_I2C.cpp
)
target_include_directories(PN532 INTERFACE
    NDEF
    PN532
    PN532_I2C
)
target_link_libraries(PN532 INTERFACE
        pico_stdlib
        hardware_i2c
)
