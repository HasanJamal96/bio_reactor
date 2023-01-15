#include <WiFi.h>
#include <HTTPClient.h>



char dataToSend[200];


void connectToWiFi() {
#if (DEBUG_MAIN == true && DEBUG_COLOR == true)
   Serial.printf("[WiFI][INFO] connceted to %s\n", ssid);
#endif
  WiFi.begin(ssid, password);
  
}

void initializeWiFi() {
  
}


bool postJsonData(char *data, char *endPoint) {
  if(WiFi.status() != WL_CONNECTED)
    return false;
  HTTPClient http;
  http.begin(endPoint);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(data);
  if(httpResponseCode > 0){
  }
  else {
    
  }
  http.end();
}


void sendError() {
  char *endPoint;
  strcpy(endPoint, serverName); 
  strcat(endPoint, ERROR_ENDPOINT);

  
  strcpy(dataToSend, "{\"UID\":\"");
  strcat(dataToSend, sampleInfo.sampleUUID);
  strcat(dataToSend, "\",\"Error_code\":\"");
  strcat(dataToSend, newError);
  strcat(dataToSend, "\",\"Datetime\":\"");
  strcat(dataToSend, "datetime add here");
  strcpy(dataToSend, "\"}");
  
  postJsonData(dataToSend, endPoint);
}


void sendDetails() {
  char *endPoint;
  strcpy(endPoint, serverName); 
  strcat(endPoint, DETAILS_ENDPOINT);
  strcpy(dataToSend, "{\"UID\":\"");
  strcat(dataToSend, sampleInfo.sampleUUID);
  strcat(dataToSend, "\",\"Red\":\"");
  strcat(dataToSend, resultEC.rgb[0].c_str());
  strcat(dataToSend, "\",\"Green\":\"");
  strcat(dataToSend, resultEC.rgb[1].c_str());
  strcat(dataToSend, "\",\"Blue\":\"");
  strcat(dataToSend, resultEC.rgb[2].c_str());
  strcat(dataToSend, "\",\"Millis\":\"");
  strcat(dataToSend, "time im millis here");
  strcat(dataToSend, "\",\"Test\":\"EC\",");
  strcat(dataToSend, "\"Datetime\":\"");
  strcat(dataToSend, "datetime add here");
  strcpy(dataToSend, "\"}");
  postJsonData(dataToSend, endPoint);
  delay(200);
  strcpy(dataToSend, "{\"UID\":\"");
  strcat(dataToSend, sampleInfo.sampleUUID);
  strcat(dataToSend, "\",\"Red\":\"");
  strcat(dataToSend, resultTC.rgb[0].c_str());
  strcat(dataToSend, "\",\"Green\":\"");
  strcat(dataToSend, resultTC.rgb[1].c_str());
  strcat(dataToSend, "\",\"Blue\":\"");
  strcat(dataToSend, resultTC.rgb[2].c_str());
  strcat(dataToSend, "\",\"Millis\":\"");
  strcat(dataToSend, "time im millis here");
  strcat(dataToSend, "\",\"Test\":\"TC\",");
  strcat(dataToSend, "\"Datetime\":\"");
  strcat(dataToSend, "datetime add here");
  strcpy(dataToSend, "\"}");
  postJsonData(dataToSend, endPoint);
}



void sendSummary() {
  char *endPoint;
  strcpy(endPoint, serverName); 
  strcat(endPoint, SUMMARY_ENDPOINT);
  postJsonData(dataToSend, endPoint);
}
