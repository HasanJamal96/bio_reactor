#include <WiFi.h>
#include <HTTPClient.h>



char dataToSend[200];


void wifiCallback(WiFiEvent_t event) {
  switch (event){
    case SYSTEM_EVENT_STA_CONNECTED:
      #if (DEBUG_MAIN == true && DEBUG_WIFI == true)
        Serial.println("[WiFi STA] Connected");
      #endif
      break;
     
    case SYSTEM_EVENT_STA_GOT_IP:
      #if (DEBUG_MAIN == true && DEBUG_WIFI == true)
        Serial.print("[WiFi STA] IP Address: ");
        Serial.println(WiFi.localIP());
        intializeNTP();
      #endif
      break;

     case SYSTEM_EVENT_AP_STACONNECTED:
      #if (DEBUG_MAIN == true && DEBUG_WIFI == true)
        Serial.println("[WiFi AP] Client connected");
      #endif
      break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
      #if (DEBUG_MAIN == true && DEBUG_WIFI == true)
        Serial.println("[WiFi AP] Client disconnected");
      #endif
      break;
   }
}

void connectToWiFi() {
#if (DEBUG_MAIN == true && DEBUG_WIFI == true)
   Serial.printf("[WiFI][INFO] connceted to %s\n", ssid);
#endif
  WiFi.begin(ssid, password);
}

void initializeWiFi() {
#if (DEBUG_MAIN == true && DEBUG_WIFI == true)
   Serial.printf("[WiFI][INFO] Initializing\n");
#endif
  WiFi.mode(WIFI_STA);
#if (DEBUG_MAIN == true && DEBUG_WIFI == true)
   Serial.printf("[WiFI][INFO] Initialized\n");
#endif
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
  const char *dateTime = (String(getUpdatedMillis())).c_str();

  
  strcpy(dataToSend, "{\"UID\":\"");
  strcat(dataToSend, sampleInfo.sampleUUID);
  strcat(dataToSend, "\",\"Error_code\":\"");
  strcat(dataToSend, newError);
  strcat(dataToSend, "\",\"Datetime\":\"");
  strcat(dataToSend, dateTime);
  strcpy(dataToSend, "\"}");
  
  postJsonData(dataToSend, endPoint);
}


void sendDetails() {
  char *endPoint;
  strcpy(endPoint, serverName); 
  strcat(endPoint, DETAILS_ENDPOINT);
  
  const char *mTime = (String(getUpdatedMillis())).c_str();
  char *dateTime = getDateTime();

  strcpy(dataToSend, "{\"UID\":\"");
  strcat(dataToSend, sampleInfo.sampleUUID);
  strcat(dataToSend, "\",\"Red\":\"");
  strcat(dataToSend, String(resultEC.rgb[0]).c_str());
  strcat(dataToSend, "\",\"Green\":\"");
  strcat(dataToSend, String(resultEC.rgb[1]).c_str());
  strcat(dataToSend, "\",\"Blue\":\"");
  strcat(dataToSend, String(resultEC.rgb[2]).c_str());
  strcat(dataToSend, "\",\"Millis\":\"");
  strcat(dataToSend, mTime);
  strcat(dataToSend, "\",\"Test\":\"EC\",");
  strcat(dataToSend, "\"Datetime\":\"");
  strcat(dataToSend, dateTime);
  strcpy(dataToSend, "\"}");
  postJsonData(dataToSend, endPoint);
  delay(200);
  strcpy(dataToSend, "{\"UID\":\"");
  strcat(dataToSend, sampleInfo.sampleUUID);
  strcat(dataToSend, "\",\"Red\":\"");
  strcat(dataToSend, String(resultTC.rgb[0]).c_str());
  strcat(dataToSend, "\",\"Green\":\"");
  strcat(dataToSend, String(resultTC.rgb[1]).c_str());
  strcat(dataToSend, "\",\"Blue\":\"");
  strcat(dataToSend, String(resultTC.rgb[2]).c_str());
  strcat(dataToSend, "\",\"Millis\":\"");
  strcat(dataToSend, mTime);
  strcat(dataToSend, "\",\"Test\":\"TC\",");
  strcat(dataToSend, "\"Datetime\":\"");
  strcat(dataToSend, dateTime);
  strcpy(dataToSend, "\"}");
  postJsonData(dataToSend, endPoint);
}



void sendSummary() {
  char *endPoint;
  strcpy(endPoint, serverName); 
  strcat(endPoint, SUMMARY_ENDPOINT);
  const char *mTime = (String(getUpdatedMillis())).c_str();
  char *dateTime = getDateTime();

  // E Coli summary
  strcpy(dataToSend, "{\"UID\":\"");
  strcat(dataToSend, sampleInfo.sampleUUID);
  if(positiveResultEC.isPositive) {
    strcat(dataToSend, "\",\"ecRed\":\"");
    strcat(dataToSend, String(positiveResultEC.rgb[0]).c_str());
    strcat(dataToSend, "\",\"ecGreen\":\"");
    strcat(dataToSend, String(positiveResultEC.rgb[1]).c_str());
    strcat(dataToSend, "\",\"ecBlue\":\"");
    strcat(dataToSend, String(positiveResultEC.rgb[2]).c_str());
    strcat(dataToSend, "\",\"ecDetectedAt\":\"");
    strcat(dataToSend, String(positiveResultEC.timeOfPositive).c_str());
  }
  else {
    strcat(dataToSend, "\",\"ecRed\":\"");
    strcat(dataToSend, String(resultEC.rgb[0]).c_str());
    strcat(dataToSend, "\",\"ecGreen\":\"");
    strcat(dataToSend, String(resultEC.rgb[1]).c_str());
    strcat(dataToSend, "\",\"ecBlue\":\"");
    strcat(dataToSend, String(resultEC.rgb[2]).c_str());
  }
  if(positiveResultTC.isPositive) {
    strcat(dataToSend, "\",\"tcRed\":\"");
    strcat(dataToSend, String(positiveResultTC.rgb[0]).c_str());
    strcat(dataToSend, "\",\"tcGreen\":\"");
    strcat(dataToSend, String(positiveResultTC.rgb[1]).c_str());
    strcat(dataToSend, "\",\"tcBlue\":\"");
    strcat(dataToSend, String(positiveResultTC.rgb[2]).c_str());
    strcat(dataToSend, "\",\"tcDetectedAt\":\"");
    strcat(dataToSend, String(positiveResultTC.timeOfPositive).c_str());
  }
  else {
    strcat(dataToSend, "\",\"tcRed\":\"");
    strcat(dataToSend, String(resultTC.rgb[0]).c_str());
    strcat(dataToSend, "\",\"tcGreen\":\"");
    strcat(dataToSend, String(resultTC.rgb[1]).c_str());
    strcat(dataToSend, "\",\"tcBlue\":\"");
    strcat(dataToSend, String(resultTC.rgb[2]).c_str());
  }
  strcat(dataToSend, "\",\"Longitude\":\"");
  strcat(dataToSend, sampleInfo.lon);
  strcat(dataToSend, "\",\"Latitude\":\"");
  strcat(dataToSend, sampleInfo.lat);
  strcat(dataToSend, "\",\"User\":\"");
  strcat(dataToSend, sampleInfo.userName);
  strcat(dataToSend, "\",\"Millis\":\"");
  strcat(dataToSend, mTime);
  strcat(dataToSend, "\",\"Test\":\"EC\",");
  strcat(dataToSend, "\"Datetime\":\"");
  strcat(dataToSend, dateTime);
  strcat(dataToSend, "\",\"Receipt_location\":\"\",\"Receipt_datetime\":\"\",\"Test_start_datetime\":\"");
  strcat(dataToSend, startTime);
  if((positiveResultEC.isPositive && positiveResultTC.isPositive) || positiveResultEC.isPositive) {
    strcat(dataToSend, "\"Test_status\":\"E\"}");
    writeResultToTag('E');
  }
  else if(positiveResultTC.isPositive) {
    strcat(dataToSend, "\"Test_status\":\"T\"}");
    writeResultToTag('T');
  }
  else {
    strcat(dataToSend, "\"Test_status\":\"F\"}");
    writeResultToTag('F');
  }

  postJsonData(dataToSend, endPoint);
}
