#include <SPI.h>
#include <MFRC522.h> // https://github.com/miguelbalboa/rfid.git

rfid_reader_t rfidReader;


MFRC522 reader(rfidReader.csPin, rfidReader.rstPin);
MFRC522::StatusCode status;

void initializeRFID() {
#if (DEBUG_MAIN == true && DEBUG_RTD == true)
  Serial.printf("[RFID][INFO] Initializing\n");
#endif

  SPI.begin(); // Init SPI bus
  reader.PCD_Init();

#if (DEBUG_MAIN == true && DEBUG_RTD == true)
  Serial.printf("[RFID][INFO] Initialized\n");
#endif
}


bool checkNewCard() {
  if ( ! reader.PICC_IsNewCardPresent()) {
    return false;
  }
  else {
#if (DEBUG_MAIN == true && DEBUG_RTD == true)
    Serial.printf("[RFID][INFO] Tag found");
#endif
  return true;
  }
}

byte buffer[18];
byte bufferSize = sizeof(buffer);




const byte sampleResultPage    = 0x07; // 1 byte at 0
const byte sampleStartTimePage = 0x04; // 10 byte
const byte sampleLatPage       = 0x08; // 10 bytes
const byte sampleLonPage       = 0x0A; // 10 bytes
const byte sampleUserPage      = 0x0D; // 12 bytes
const byte sampleTestTypePage  = 0x07; // 1 byte at 3

const byte writeSize = 4;



bool writeResultToTag(byte data) {
  byte writeBuffer[6];
  writeBuffer[0] = data;
  writeBuffer[3] = sampleInfo.sampleType;
  status = (MFRC522::StatusCode) reader.MIFARE_Ultralight_Write(sampleResultPage, &writeBuffer[0], 4);
  if (status != MFRC522::STATUS_OK) {
    #if (DEBUG_MAIN == true && DEBUG_RTD == true)
      Serial.printf("[RFID][INFO] Write failed");
    #endif
    return false;
  }
  return true;
}

bool writeStartTimeToTag(String data) {
  byte writeBuffer[14];
  data.getBytes(writeBuffer, data.length());
  for(uint8_t i=0; i<3; i++) {
    status = (MFRC522::StatusCode) reader.MIFARE_Ultralight_Write(sampleStartTimePage+i, &writeBuffer[i*4], 4);
    if (status != MFRC522::STATUS_OK) {
      #if (DEBUG_MAIN == true && DEBUG_RTD == true)
        Serial.printf("[RFID][INFO] Write failed");
      #endif
      return false;
    }
  }
  return true;
}

void readUid() {
  String x = "";  
  for (byte i = 0; i < 4; i++) {
    x += String(reader.uid.uidByte[i], HEX);
  }
  strcpy(sampleInfo.sampleUUID, x.c_str());
  #if (DEBUG_MAIN == true && DEBUG_RTD == true)
    Serial.printf("[RFID][INFO] UID is: %s\n", sampleInfo.sampleUUID);
  #endif
}

bool readUsername() {
  status = (MFRC522::StatusCode) reader.MIFARE_Read(sampleUserPage, buffer, &bufferSize);
  if (status != MFRC522::STATUS_OK) {
    #if (DEBUG_MAIN == true && DEBUG_RTD == true)
      Serial.printf("[RFID][INFO] Read failed");
    #endif
    return false;
  }
  for(uint8_t i=0; i<12; i++) {
    sampleInfo.userName[i] = (char)buffer[i];
  }
  #if (DEBUG_MAIN == true && DEBUG_RTD == true)
    Serial.printf("[RFID][INFO] Username is: %s\n", sampleInfo.userName);
  #endif
  return true;
}

byte readTestType() {
  status = (MFRC522::StatusCode) reader.MIFARE_Read(sampleTestTypePage, buffer, &bufferSize);
  if (status != MFRC522::STATUS_OK) {
    #if (DEBUG_MAIN == true && DEBUG_RTD == true)
      Serial.printf("[RFID][INFO] Read failed");
    #endif
    return false;
  }
  #if (DEBUG_MAIN == true && DEBUG_RTD == true)
    Serial.printf("[RFID][INFO] Test type is: %s\n", (char)buffer[3]);
  #endif
  return buffer[0];
}

byte readTestStatus() {
  status = (MFRC522::StatusCode) reader.MIFARE_Read(sampleResultPage, buffer, &bufferSize);
  if (status != MFRC522::STATUS_OK) {
    #if (DEBUG_MAIN == true && DEBUG_RTD == true)
      Serial.printf("[RFID][INFO] Read failed");
    #endif
    return 0;
  }
  #if (DEBUG_MAIN == true && DEBUG_RTD == true)
    Serial.printf("[RFID][INFO] Test status is: %s\n", (char)buffer[0]);
  #endif
  return buffer[0];
}

bool readLocation() {
  status = (MFRC522::StatusCode) reader.MIFARE_Read(sampleLatPage, buffer, &bufferSize);
  if (status != MFRC522::STATUS_OK) {
    #if (DEBUG_MAIN == true && DEBUG_RTD == true)
      Serial.printf("[RFID][INFO] Read failed");
    #endif
    return false;
  }
  for(uint8_t i=0; i<10; i++) {
    sampleInfo.lat[i] = (char)buffer[i];
  }
  status = (MFRC522::StatusCode) reader.MIFARE_Read(sampleLonPage, buffer, &bufferSize);
  if (status != MFRC522::STATUS_OK) {
    #if (DEBUG_MAIN == true && DEBUG_RTD == true)
      Serial.printf("[RFID][INFO] Read failed");
    #endif
    return false;
  }
  for(uint8_t i=2; i<12; i++) {
    sampleInfo.lat[i-2] = (char)buffer[i];
  }
  #if (DEBUG_MAIN == true && DEBUG_RTD == true)
    Serial.printf("[RFID][INFO] Location: %s,%s\n", sampleInfo.lat, sampleInfo.lon);
  #endif
  return true;
}


bool readTag() {
  newTag = false;
  if(checkNewCard()) {
    MFRC522::StatusCode status;
    if ( ! reader.PICC_ReadCardSerial())
      return false;
    byte x;
    x = readTestStatus();
    if(x != 0) {
      if(x == 'N') {
        x = readTestType();
        if(x != 0) {
          if(x == 'A') {
            sampleInfo.sampleType = COLITAG;
          }
          else if(x == 'B') {
            sampleInfo.sampleType = COLILERT;
          }
          else if(x == 'C') {
            sampleInfo.sampleType = COLILERT18;
          }
          else
            return false;
          if(readUsername())
            if(readLocation())
              readUid();
            else
              return false;
          else
            return false;
        }
        else
          return false;
        return true;
      }
      else {
        newTag = true;
        #if (DEBUG_MAIN == true && DEBUG_RTD == true)
          Serial.printf("[RFID][INFO] Process already performed\n");
        #endif
        return false;
      }
    }
  }
  else {
    return false;
  }
}
