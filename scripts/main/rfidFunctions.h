#include <SPI.h>
#include <MFRC522.h> // https://github.com/miguelbalboa/rfid.git

rfid_reader_t rfidReader;


MFRC522 reader(rfidReader.csPin, rfidReader.rstPin);
MFRC522::MIFARE_Key key;

void initializeRFID() {
#if (DEBUG_MAIN == true && DEBUG_RTD == true)
  Serial.printf("[RFID][INFO] Initializing\n");
#endif

  SPI.begin(); // Init SPI bus
  reader.PCD_Init();
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

#if (DEBUG_MAIN == true && DEBUG_RTD == true)
  Serial.printf("[RFID][INFO] Initialized\n");
#endif
}


bool checkNewCard() {
  if ( ! reader.PICC_IsNewCardPresent()) {
#if (DEBUG_MAIN == true && DEBUG_RTD == true)
    Serial.printf("[RFID][INFO] Tag not found");
#endif  
  return false;
  }
  else {
#if (DEBUG_MAIN == true && DEBUG_RTD == true)
    Serial.printf("[RFID][INFO] Tag found");
#endif
  return true;
  }
}


bool readTag() {
  if(checkNewCard()) {
    MFRC522::StatusCode status;
    if ( ! reader.PICC_ReadCardSerial())
      return false;
    #if (DEBUG_MAIN == true && DEBUG_RTD == true)
      Serial.printf("[RFID][INFO] Checking tag compatibility");
    #endif
    MFRC522::PICC_Type piccType = reader.PICC_GetType(reader.uid.sak);
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
      #if (DEBUG_MAIN == true && DEBUG_RTD == true)
        Serial.printf("[RFID][INFO] Compatibility failed");
      #endif
        return false;
    }
    else {
      #if (DEBUG_MAIN == true && DEBUG_RTD == true)
        Serial.printf("[RFID][INFO] Authenticating using key A...");
      #endif
      status = reader.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(reader.uid)); 
      if (status != MFRC522::STATUS_OK) {
        #if (DEBUG_MAIN == true && DEBUG_RTD == true)
          Serial.printf("[RFID][INFO] Authenticating failed");
        #endif
        return false;
      }
      else {
        byte buffer[18];
        uint8_t len = 18;
        uint8_t block = 4;
        status = reader.MIFARE_Read(block, buffer, &len);
        char readData[17];
        for (uint8_t i = 0; i < 16; i++) {
          if (buffer[i] != 32) {
            readData[i] = (char)buffer[i];          
          }
          else {
            readData[i] = '\0';
            break;
          }
        }
        #if (DEBUG_MAIN == true && DEBUG_RTD == true)
          Serial.printf("[RFID][INFO] Data read from tag: %s\n", readData);
        #endif
        if(strcmp(readData, "tested") == 0) {
          #if (DEBUG_MAIN == true && DEBUG_RTD == true)
            Serial.printf("[RFID][INFO] Process already performed\n");
          #endif
          return false;
        }
        else if(strcmp(readData, "not tested") == 0) {
          return true;
        }
      }
    }
  }
  else {
    return false;
  }
  // depends on the type of card client using
}