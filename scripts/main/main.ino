#include "config.h"
#include "rfidFunctions.h"
#include "heaterFunctions.h"
#include "temperatureFunctions.h"
#include "colorSensorFunctions.h"

main_process_t  process;
result_t        result;
led_illumination_t  leds;
void setup() {
#if (DEBUG_MAIN == true)
  Serial.begin(BAUDRATE);
  Serial.printf("[Main][INFO] Setup started\n");
  Serial.printf("[Main][INFO] Firmware version %s\n", FIRMWARE_VERSION);
#endif

  initializeRFID();
  initializeHeater();
  initializeTemperatureSensor();

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

void loop() {
  if(!process.started) {
    if(readTag()) {
      process.started = true;
      process.stage = CALCULATING_TIME;
    }   
  }
  else {
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
      float tol = getTolerance();
      float currentTemperature = getTemperature();
      if (process.stage == COLLING) {
        if(currentTemperature < 35 + tol && currentTemperature > 35 - tol) {
          process.stage = MAINTANING_TEMP;
        }
      }
      else if(process.stage == MAINTANING_TEMP) {
        if(currentTemperature >= 35 + tol) {
          stopHeater();
        }
        else if(currentTemperature <= 35 - tol) {
          updateHeaterDuty(50);   // 20% duty cycle
        }
        unsigned long currentTime = millis();
        if(currentTime - leds.lastWhiteOnTime >= leds.WHITE_INTERVAL_AFTER) {
          if(isLidOpen()) {
            // alert user
          }
          else {
            ledWhite(true);
            delay(30);
            updateColor();
            ledWhite(false);
            leds.lastWhiteOnTime = currentTime;
            // add color conditions
          }
        }
        if(currentTime - leds.lastUVOnTime >= leds.UV_INTERVAL_AFTER) {
          if(isLidOpen()) {
            // alert user
          }
          else {
            ledWhite(true);
            delay(30);
            updateColor();
            ledWhite(false); 
            leds.lastUVOnTime = currentTime;
            // add color conditions
          }
        }
        
      }
    } 
  }
}