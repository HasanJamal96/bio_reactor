heater_t heater;

void initializeHeater() {
#if (DEBUG_MAIN == true && DEBUG_HEATER == true)
  Serial.printf("[Heater][INFO] Initializing\n");
#endif

  ledcSetup(heater.channel , heater.frequency, heater.resolution);
  ledcAttachPin(heater.pin, heater.channel);

#if (DEBUG_MAIN == true && DEBUG_HEATER == true)
  Serial.printf("[Heater][INFO] Initialized\n");
#endif
}

float getThermalPower() {
  return heater.thermalPower;
}


void updateHeaterDuty(uint8_t duty) {
  if(duty == heater.duty && heater.state) {
    return;
  }
  heater.state = true;
  heater.duty = duty;
  ledcWrite(heater.channel, heater.duty);

#if (DEBUG_MAIN == true && DEBUG_HEATER == true)
  Serial.printf("[Heater][INFO] Duty updated\n");
#endif
}

void stopHeater() {
  if(heater.state) {
    updateHeaterDuty(0);
    heater.state = false;
#if (DEBUG_MAIN == true && DEBUG_HEATER == true)
  Serial.printf("[Heater][INFO] Stopped\n");
#endif
  }
#if (DEBUG_MAIN == true && DEBUG_HEATER == true)
  else {
  Serial.printf("[Heater][INFO] Already stopped\n");
#endif

  }
}