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

void rawToHSV() {
  float r = float(color.value[0]/65535);
  float g = float(color.value[1]/65535);
  float b = float(color.value[2]/65535);
  float h;
  
  float high = max(r, g);
  high = max(high, b);
  float low = min(r, g);
  low = min(low, b);
  color.hsv[0] = high;
  color.hsv[1] = high;
  color.hsv[2] = high;
  
  float d = high - low;
  
  if (high == 0)
    color.hsv[1] = 0;
  else
    color.hsv[1] = d/high;
  if (high == low)
      color.hsv[0] = 0.0;
  else {
    if(high == r) {
      h = (g - b) / d + (g < b ? 6:0);
    }
    else if(high == g) {
      h = (b - r) / d+2;
    }
    else if(high == b) {
      h = (r - g) / d+4;
    }
  }
  h /= 6;
  
  color.hsv[0] = h*360.0;
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
  rawToHSV();
  return true;
}
