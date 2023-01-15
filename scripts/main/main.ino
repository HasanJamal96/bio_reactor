#include "config.h"

main_process_t      process;
result_t            resultEC;
result_t            resultTC;
led_illumination_t  leds;

comparator_color_t  colorComparator;
sample_info_t       sampleInfo;



#include "button.h"
#include "rfidFunctions.h"
#include "heaterFunctions.h"
#include "temperatureFunctions.h"
#include "colorSensorFunctions.h"
#include "apiCommunication.h"
#include "rgb.h"




Button btn(32, 25); // pin number, debounce

void setup() {
#if (DEBUG_MAIN == true)
  Serial.begin(BAUDRATE);
  Serial.printf("[Main][INFO] Setup started\n");
  Serial.printf("[Main][INFO] Firmware version %s\n", FIRMWARE_VERSION);
#endif

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
  float thermalPower       = getThermalPower();
  process.totalTime = (thermalPower * 3600) / (4.2 * (35 - currentTemperature));
  
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

void loop() {
  btn.read();
  if(btn.pressedFor(1000)) {
    if(!process.started)
      process.started = true;
  }
  if(process.started) {
    if(readTag()) {
      resultTC.rgb[0] = resultTC.rgb[1] = resultTC.rgb[2] = 255;
      resultEC.rgb[0] = resultEC.rgb[1] = resultEC.rgb[2] = 255;
      tagFound = true;
      process.stage = CALCULATING_TIME;
    }
    if(tagFound) {
      if(process.stage == CALCULATING_TIME) {
        calculateProcessTime();
      }
      else if(process.stage == HEATING) {
        float currentTemperature = getTemperature();
        if(currentTemperature >= 44.5) {
          process.stage = COLLING;
          updateHeaterDuty(50);   // 20% duty cycle
        }
      }
      else {
        if(millis() - process.startTime >= process.totalTime) {
          updateColor();
          // calculate result
          // result can be negative or positive based on color 
        }
        if(millis() - lastDetailPosted >= detailsUpdateInterval) {
          sendDetails();
          lastDetailPosted = millis();
        }
        float tol = getTolerance();
        float currentTemperature = getTemperature();
        if (process.stage == COLLING) {
          if(currentTemperature < 35 + tol && currentTemperature > 35 - tol) {
            process.stage = MAINTANING_TEMP;
          }
        }
        else if(process.stage == MAINTANING_TEMP) {
          if(currentTemperature >= 35 + (2*tol)) {
            strcpy(newError, "HighTemp");
            if(strcmp(oldError, newError) != 0) {
              strcpy(oldError, newError);
              sendError();
            }
          }
          else if(currentTemperature <= 35 - (2*tol)) {
            strcpy(newError, "LowTemp");
            if(strcmp(oldError, newError) != 0) {
              strcpy(oldError, newError);
              sendError();
            }
          }
          else if(currentTemperature >= 35 + tol) {
            stopHeater();
          }
          else if(currentTemperature <= 35 - tol) {
            updateHeaterDuty(50);   // 20% duty cycle
          }
          unsigned long currentTime = millis();
          if(currentTime - leds.lastWhiteOnTime >= leds.WHITE_INTERVAL_AFTER) {
            if(isLidOpen()) {
              startFlashing();
              strcpy(newError, "LidOpen");
              if(strcmp(oldError, newError) != 0) {
                strcpy(oldError, newError);
                sendError();
              }
            }
            else {
              ledWhite(true);
              delay(30);
              updateColor();
              ledWhite(false);
              leds.lastWhiteOnTime = currentTime;
              // add color conditions
              /*
              check if rgb values is in between yellow or darker yellow
              if is in range test is positive
              */
            }
          }
          if(currentTime - leds.lastUVOnTime >= leds.UV_INTERVAL_AFTER) {
            if(isLidOpen()) {
              startFlashing();
              strcpy(newError, "LidOpen");
              if(strcmp(oldError, newError) != 0) {
                strcpy(oldError, newError);
                sendError();
              }
            }
            else {
              ledUv(true);
              delay(30);
              updateColor();
              ledUv(false); 
              leds.lastUVOnTime = currentTime;
              // add color conditions
            }
          }
          
        }
      } 
    }
  }
}
