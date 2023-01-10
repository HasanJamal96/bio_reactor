#include <Adafruit_MAX31865.h>

temperature_t temperature;


Adafruit_MAX31865 thermo = Adafruit_MAX31865(temperature.csPin, temperature.mosiPin, temperature.misoPin, temperature.clkPin);


void initializeTemperatureSensor() {
#if (DEBUG_MAIN == true && DEBUG_RTD == true)
  Serial.printf("[Heater][INFO] Initializing\n");
#endif

  thermo.begin(MAX31865_2WIRE);

#if (DEBUG_MAIN == true && DEBUG_RTD == true)
  Serial.printf("[Heater][INFO] Initialized\n");
#endif
}

void updateThresholds(uint16_t lower, uint16_t upper) {
  thermo.setThresholds(lower, upper);
#if (DEBUG_MAIN == true && DEBUG_RTD == true)
  Serial.printf("[Heater][INFO] Threshold updated lower:%d, upper:%d\n", lower, upper);
#endif
}

void readTemperature() {
#if (DEBUG_MAIN == true && DEBUG_RTD == true)
  Serial.printf("[Heater][INFO] Reading temperature\n");
#endif
  temperature.temp = thermo.temperature(temperature.RNOMINAL, temperature.RREF);
  uint8_t fault = thermo.readFault();
  if (fault) {
#if (DEBUG_MAIN == true && DEBUG_RTD == true)
    Serial.printf("[Heater][ERROR] Fault 0x"); 
    Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.printf("[Heater][ERROR] RTD High Threshold\n"); 
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.printf("[Heater][ERROR] RTD Low Threshold\n"); 
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.printf("[Heater][ERROR] REFIN- > 0.85 x Bias\n"); 
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.printf("[Heater][ERROR] REFIN- < 0.85 x Bias - FORCE- open\n"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.printf("[Heater][ERROR] RTDIN- < 0.85 x Bias - FORCE- open\n"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.printf("[Heater][ERROR] Under/Over voltage\n"); 
    }
#endif
    thermo.clearFault();
  }  
}

float getTemperature() {
  readTemperature();
  return temperature.temp;
}

float getTolerance() {
  return temperature.TOLERANCE;
}
