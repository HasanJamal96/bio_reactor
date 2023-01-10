#include "Wire.h"
#include "veml6040.h" // https://github.com/thewknd/VEML6040.git

colot_t   color;
VEML6040  RGBWSensor;

void initializeColorSensor() {
#if (DEBUG_MAIN == true && DEBUG_COLOR == true)
  Serial.printf("[Color][INFO] Initializing\n");
#endif
  Wire.begin();
  if(!RGBWSensor.begin()) {
    color.working = false;
#if (DEBUG_MAIN == true && DEBUG_COLOR == true)
    Serial.printf("[Color][ERROR] Couldn't detect the sensor\n");
#endif
  }
  else {
    RGBWSensor.setConfiguration(VEML6040_IT_160MS + VEML6040_TRIG_ENABLE + VEML6040_AF_FORCE + VEML6040_SD_ENABLE);
    color.working = true;
    delay(1000);
#if (DEBUG_MAIN == true && DEBUG_COLOR == true)
    Serial.printf("[Color][INFO] Initialized\n");
#endif
  }
}

bool isLidOpen() {
  RGBWSensor.setConfiguration(VEML6040_IT_160MS + VEML6040_TRIG_ENABLE + VEML6040_AF_FORCE + VEML6040_SD_ENABLE);
  uint16_t lux = RGBWSensor.getAmbientLight();
  if(lux >= color.MAX_LUX)
    return true;
  else
    return false;
}

bool updateColor() {
  if(!color.working) {
    return false;
  }
  
#if (DEBUG_MAIN == true && DEBUG_COLOR == true)
  Serial.printf("[Color][INFO] Getting color value\n");
#endif
  RGBWSensor.setConfiguration(VEML6040_IT_160MS + VEML6040_TRIG_ENABLE + VEML6040_AF_FORCE + VEML6040_SD_ENABLE);

  color.value[0] = RGBWSensor.getRed();
  color.value[1] = RGBWSensor.getGreen();
  color.value[2] = RGBWSensor.getBlue();
  color.value[3] = RGBWSensor.getWhite();
  return true;
}