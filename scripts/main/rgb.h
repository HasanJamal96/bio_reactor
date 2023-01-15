rgb_t rgbLed;


void initializeRGB() {
  for(uint8_t i=0; i<3; i++)
    pinMode(rgbLed.Pins[i], OUTPUT);
}

void closeLed() {
  for(uint8_t i=0; i<3; i++)
    digitalWrite(rgbLed.Pins[i], LOW);
}

void toggleLed() {
  rgbLed.flashing = !rgbLed.flashing;
  if(rgbLed.flashing) {
    for(uint8_t i=0; i<3; i++)
      digitalWrite(rgbLed.Pins[i], HIGH);
  }
  else {
    closeLed();
  }
};

void startFlashing() {
  rgbLed.errorState = true;
}

void stopFlashing() {
  rgbLed.errorState = false;
}


void rgbLoop() {
  if(rgbLed.flashing && !rgbLed.errorState) {
    closeLed();
  }
  if(rgbLed.errorState) {
    if(millis() - rgbLed.lastBlink >= rgbLed.blinkDelay) {
      toggleLed();
      rgbLed.lastBlink = millis();
    }
  }
}
