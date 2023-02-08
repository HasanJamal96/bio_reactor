#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

const char* ntpServer = "pool.ntp.org";
const int   utcOffsetInSeconds = 3600;


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, utcOffsetInSeconds);

void intializeNTP() {
  timeClient.begin();
  #if (DEBUG_MAIN == true && DEBUG_WIFI == true)
    Serial.println("[NTP] Initialized");
  #endif
}

unsigned long getUpdatedMillis() {
  if(timeClient.update()) {
    return timeClient.getEpochTime();
  }
  return 0;
}

char * getDateTime() {
  unsigned long t = getUpdatedMillis();
  char buff[32];
  sprintf(buff, "%02d.%02d.%02d %02d:%02d:%02d", year(t), month(t), day(t), hour(t), minute(t), second(t));
  return buff;
}