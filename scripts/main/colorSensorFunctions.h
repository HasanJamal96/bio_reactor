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

bool hsvToRGB() {
  if(color.hsv[0] > 360 || color.hsv[0] < 0 || color.hsv[1] > 100 || color.hsv[1] < 0 || color.hsv[2] < 0 || color.hsv[2] > 100) {
#if (DEBUG_MAIN == true && DEBUG_COLOR == true)
   Serial.printf("[Color][ERROR] Invalid HSV values\n");
#endif
  return false;
  }
  float s = color.hsv[1]/100;
  float v = color.hsv[2]/100;
  float C = s*v;
  float X = C*(1-abs(fmod(color.hsv[0]/60.0, 2)-1));
  float m = v-C;
  float r,g,b;
  if(color.hsv[0] >= 0 && color.hsv[0] < 60){
      r = C,g = X,b = 0;
  }
  else if(color.hsv[0] >= 60 && color.hsv[0] < 120){
      r = X,g = C,b = 0;
  }
  else if(color.hsv[0] >= 120 && color.hsv[0] < 180){
      r = 0,g = C,b = X;
  }
  else if(color.hsv[0] >= 180 && color.hsv[0] < 240){
      r = 0,g = X,b = C;
  }
  else if(color.hsv[0] >= 240 && color.hsv[0] < 300){
      r = X,g = 0,b = C;
  }
  else{
      r = C,g = 0,b = X;
  }
  color.rgb[0] = (r+m)*255;
  color.rgb[1] = (g+m)*255;
  color.rgb[2] = (b+m)*255;
  return true;
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
