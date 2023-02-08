#include "config.h"

main_process_t      process;
result_t            resultEC; // saves E coli test's current values
result_t            resultTC; // saves T coli test's current values
result_t            positiveResultEC; // saves E coli result if test is positive
result_t            positiveResultTC; // saves T coli result if test is positive
led_illumination_t  leds;

comparator_color_t  colorComparator;
sample_info_t       sampleInfo;
bool newTag = false;
char *startTime;

#include "button.h"
#include "rfidFunctions.h"
#include "heaterFunctions.h"
#include "temperatureFunctions.h"
#include "colorSensorFunctions.h"
#include "ntp.h"
#include "apiCommunication.h"
#include "rgb.h"





Button btn(32, 25); // pin number, debounce

void setup() {
#if (DEBUG_MAIN == true)
  Serial.begin(BAUDRATE);
  Serial.printf("[Main][INFO] Setup started\n");
  Serial.printf("[Main][INFO] Firmware version %s\n", FIRMWARE_VERSION);
#endif

  initializeWiFi();
  connectToWiFi();
  initializeRFID();
  initializeHeater();
  initializeTemperatureSensor();
  btn.begin();

  pinMode(W_LED_PIN, OUTPUT);
  pinMode(UV_LED_PIN, OUTPUT);

#if (DEBUG_MAIN == true)
  Serial.printf("[Main][INFO] Setup complete\n");
#endif
}

void calculateProcessTime() {
  float currentTemperature = getTemperature();
  /*float thermalPower       = getThermalPower();
  process.totalTime = (thermalPower * 3600) / (4.2 * (35 - currentTemperature));*/

  if(sampleInfo.sampleType == COLITAG) {
    process.totalTime = 16 * 3600000; // 1 hrs 3600000 milli sec
  }
  else if(sampleInfo.sampleType == COLILERT) {
    process.totalTime = 24 * 3600000; // 1 hrs 3600000 milli sec
  }
  else if(sampleInfo.sampleType == COLILERT18) {
    process.totalTime = 18 * 3600000; // 1 hrs 3600000 milli sec
  }
  
  if(currentTemperature >= 44.5) {
    process.stage = COLLING;
  }
  else {
    process.stage = HEATING;
    updateHeaterDuty(127); // 50 % duty cycle
  }
}

void ledWhite(bool on) {
  if(on)
    digitalWrite(W_LED_PIN, HIGH);
  else
    digitalWrite(W_LED_PIN, LOW);
}

void ledUv(bool on) {
  if(on)
    digitalWrite(UV_LED_PIN, HIGH);
  else
    digitalWrite(UV_LED_PIN, LOW);
}

bool tagFound = false;

unsigned long lastDetailPosted = 0;
const uint16_t detailsUpdateInterval = 60000;

bool compareColors(uint8_t performTestType) {
  bool positive = false;
  switch(sampleInfo.sampleType) { // if all RGB (values) are less  or equal to comparator test is positive
    case COLITAG:
      if(color.rgb[0] <= colorComparator.COLITAG[performTestType][0]) {
        if(color.rgb[1] <= colorComparator.COLITAG[performTestType][1]) {
          if(color.rgb[2] <= colorComparator.COLITAG[performTestType][2]) {
            positive = true;
          }
        }
      }
      break;
    case COLILERT:
      if(color.rgb[0] <= colorComparator.COLILERT[performTestType][0]) {
        if(color.rgb[1] <= colorComparator.COLILERT[performTestType][1]) {
          if(color.rgb[2] <= colorComparator.COLILERT[performTestType][2]) {
            positive = true;
          }
        }
      }
      break;
    case COLILERT18:
      if(color.rgb[0] <= colorComparator.COLILERT18[performTestType][0]) {
        if(color.rgb[1] <= colorComparator.COLILERT18[performTestType][1]) {
          if(color.rgb[2] <= colorComparator.COLILERT18[performTestType][2]) {
            positive = true;
          }
        }
      }
      break;
  }
  if(performTestType == 0) {
    resultEC.rgb[0] = color.rgb[0];
    resultEC.rgb[1] = color.rgb[1];
    resultEC.rgb[2] = color.rgb[2];
  }
  else {
    resultTC.rgb[0] = color.rgb[0];
    resultTC.rgb[1] = color.rgb[1];
    resultTC.rgb[2] = color.rgb[2];
  }
  return positive;
}


void loop() {
  btn.read();
  if(btn.pressedFor(1000)) {
    if(!process.started)
      process.started = true;
  }
  if(process.started) {
    rgbLoop();
    if(strcmp(newError, "LidOpen")) {
      if(!isLidOpen()) {
        startFlashing();
        strcpy(newError, "LidOpen");
        if(strcmp(oldError, newError) != 0) {
          strcpy(oldError, newError);
          sendError();
          rgbLed.errorState = true;
        }
      }
    }
    if(readTag()) {
      resultTC.rgb[0] = resultTC.rgb[1] = resultTC.rgb[2] = 255;
      resultEC.rgb[0] = resultEC.rgb[1] = resultEC.rgb[2] = 255;
      positiveResultTC.isPositive = false;
      positiveResultEC.isPositive = false;
      tagFound = true;
      process.stage = CALCULATING_TIME;
      writeResultToTag('S');  // started -> S
      startTime = getDateTime();
      writeStartTimeToTag(String(startTime));
    }
    else {
      if(newTag) {
        startFlashing();
        newTag = false;
      }
    }
    if(tagFound) {
      if(process.stage == CALCULATING_TIME) {
        calculateProcessTime();
      }
      else if(process.stage == HEATING) {
        float currentTemperature = getTemperature();
        if(currentTemperature >= 44.5) {
          process.stage = COLLING;
          updateHeaterDuty(25);   // 10% duty cycle
        }
      }
      else {
        if(millis() - process.startTime >= process.totalTime) {
          sendSummary();
          process.started = false;
        }
        else if(millis() - lastDetailPosted >= detailsUpdateInterval) {
          sendDetails();
          lastDetailPosted = millis();
        }
        float tol = getTolerance();
        float currentTemperature = getTemperature();
        if (process.stage == COLLING) {
          if(currentTemperature < 35 + tol && currentTemperature > 35 - tol) {
            process.stage = MAINTANING_TEMP;
          }
        } // lets  say tolerance is 0.5
        else if(process.stage == MAINTANING_TEMP) {
          if(currentTemperature >= 35 + (2*tol)) { // if temperature rises to 36
            strcpy(newError, "HighTemp");
            if(strcmp(oldError, newError) != 0) {
              strcpy(oldError, newError);
              sendError();
              rgbLed.errorState = true;
            }
          }
          else if(currentTemperature <= 35 - (2*tol)) { // if temperature falls to 34
            strcpy(newError, "LowTemp");
            if(strcmp(oldError, newError) != 0) {
              strcpy(oldError, newError);
              sendError();
              rgbLed.errorState = true;
            }
          }
          else if(currentTemperature >= 35 + tol) { // if temperature rises to 35.5
            stopHeater();
          }
          else if(currentTemperature <= 35 - tol) { // if temperature falls to 34.5
            updateHeaterDuty(25);   // 10% duty cycle
          }
          unsigned long currentTime = millis();
          if(currentTime - leds.lastWhiteOnTime >= leds.WHITE_INTERVAL_AFTER) {
            if(isLidOpen()) {
              startFlashing();
              strcpy(newError, "LidOpen");
              if(strcmp(oldError, newError) != 0) {
                strcpy(oldError, newError);
                sendError();
                rgbLed.errorState = true;
              }
            }
            else {
              ledWhite(true);
              delay(30);
              bool colorUpdated = updateColor();
              ledWhite(false);
              leds.lastWhiteOnTime = currentTime;
              if(colorUpdated) {
                if(compareColors(0)) { // 0 means E. coli
                  positiveResultEC.isPositive = true;
                  positiveResultEC.rgb[0] = color.rgb[0];
                  positiveResultEC.rgb[1] = color.rgb[1];
                  positiveResultEC.rgb[2] = color.rgb[2];
                  positiveResultEC.timeOfPositive = millis(); // have to add current time in millis
                }
              }
              else {
                rgbLed.errorState = true;
              }
            }
          }
          if(currentTime - leds.lastUVOnTime >= leds.UV_INTERVAL_AFTER) {
            if(isLidOpen()) {
              startFlashing();
              strcpy(newError, "LidOpen");
              if(strcmp(oldError, newError) != 0) {
                strcpy(oldError, newError);
                sendError();
                rgbLed.errorState = true;
              }
            }
            else {
              ledUv(true);
              delay(30);
              bool colorUpdated = updateColor();
              ledUv(false); 
              leds.lastUVOnTime = currentTime;
              if(colorUpdated) {
                if(compareColors(1)) { // 0 means T. Coliform
                  positiveResultTC.isPositive = true;
                  positiveResultTC.rgb[0] = color.rgb[0];
                  positiveResultTC.rgb[1] = color.rgb[1];
                  positiveResultTC.rgb[2] = color.rgb[2];
                  positiveResultTC.timeOfPositive = millis(); // have to add current time in millis
                }
              }
              else {
                rgbLed.errorState = true;
              }
            }
          }
        }
      } 
    }
  }
}
