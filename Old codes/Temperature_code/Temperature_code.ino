
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <time.h>

#include <Adafruit_MAX31865.h>

// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 thermo = Adafruit_MAX31865(5, 23, 19, 18);
// use hardware SPI, just pass in the CS pin
//Adafruit_MAX31865 thermo = Adafruit_MAX31865(10);

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0


const int relay = 26;
const char* ssid     = "xxxxx";
const char* password = "xxxx";

const char* ntpServer = "nz.pool.ntp.org";
const long  gmtOffset_sec = 43200;
const int   daylightOffset_sec = 3600;

//Define calibration factor for Sensor 1
float RawHigh1 = 99.6;
float RawLow1 = 0.5;
float ReferenceHigh = 99.9;
float ReferenceLow = 0;
float RawRange1 = RawHigh1 - RawLow1;
float ReferenceRange = ReferenceHigh - ReferenceLow;



//Define calibration factor for Sensor 2
float RawHigh2 = 99.6;
float RawLow2 = 0.5;
float RawRange2 = RawHigh2 - RawLow2;
// reference ranges are same as sensor1

//define temperature stages
float temp1 = 35.86;//estimated to heat water to 35 degrees
float temp;
float temp2 = 46.4;//estimated to heat water to 44.5 degrees
float time2 = 7200000;

// Data wire is plugged into port 15 on the ESP32
#define ONE_WIRE_BUS 15

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

uint8_t sensor1[8] = { 0x28, 0xD8, 0xF2, 0x76, 0xE0, 0x01, 0x3C, 0xE1  };

float tempSensor1;

uint8_t sensor2[8] = { 0x28, 0xCC, 0xBE, 0x76, 0xE0, 0x01, 0x3C, 0xAE  };

float tempSensor2;
 
void setup() {
  // start serial port
  Serial.begin(115200);
    // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  
  //Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();Serial.println("");

//  disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  
  // Start up the library
  sensors.begin();
  
  thermo.begin(MAX31865_2WIRE); 

  pinMode(relay, OUTPUT); 

}
void loop() {
if (millis()<time2) {
  temp = temp1;
}
else {
  temp = temp2; 
}
 
  if (thermo.temperature(RNOMINAL, RREF) < temp) {digitalWrite(relay, HIGH);} 
 

else {
digitalWrite(relay, LOW); 
}

  sensors.requestTemperatures();// Send the command to get temperatures
  tempSensor1 = sensors.getTempC(sensor1); // Gets the values of the temperature
  tempSensor2 = sensors.getTempC(sensor2); // Gets the values of the temperature

//  printLocalTime(); 
//  Serial.print("\t");  Serial.print("Temperature Sensor 1: "); Serial.print("\t");  Serial.println((((tempSensor1 - RawLow1) * ReferenceRange) / RawRange1) + ReferenceLow);
  printLocalTime(); 
  Serial.print("\t"); Serial.print("Water temp: "); Serial.print("\t");  Serial.print((((tempSensor2 - RawLow2) * ReferenceRange) / RawRange2) + ReferenceLow);Serial.print("\t"); Serial.print("Incubator temp: "); Serial.print("\t"); Serial.println(thermo.temperature(RNOMINAL, RREF));

  delay(100);
}


void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.print("Failed to obtain time");
    return;
  }
  Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S");

}
