 // TCS230 or TCS3200 pins wiring to Arduino
#define S0 19
#define S1 18
#define S2 2
#define S3 4
#define sensorOut 15

#include <Wire.h> 
const byte ecoli = 32;
const byte coliforms = 33;

 

// Stores frequency read by the photodiodes
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

void setup() {
  // Setting the outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode(ecoli, OUTPUT);
  pinMode(coliforms, OUTPUT);
  
  // Setting the sensorOut as an input
  pinMode(sensorOut, INPUT);
  
  // Setting frequency scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
  
   // Begins serial communication 
  Serial.begin(115200);
  delay(1000);
  Serial.print("Red");Serial.print("\t");Serial.print("Green");Serial.print("\t");Serial.print("Blue");Serial.print("\t");Serial.print("Test");Serial.print("\t");Serial.println("Milliseconds");
}
void loop() {
  
  //Check ecoli fluoressence using UV light
  digitalWrite(ecoli, HIGH);   // turn the UV LED on 
  delay(100);
  
  // Setting RED (R) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  
  // Reading the output frequency
  redFrequency = pulseIn(sensorOut, LOW);
  
   // Printing the RED (R) value
  //Serial.print("R = ");Serial.print("\t");
  Serial.print(redFrequency);Serial.print("\t");
 // delay(100);
  
  // Setting GREEN (G) filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  
  // Reading the output frequency
  greenFrequency = pulseIn(sensorOut, LOW);
  
  // Printing the GREEN (G) value  
  //Serial.print(" G = ");Serial.print("\t");
  Serial.print(greenFrequency);Serial.print("\t");
 // delay(100);
 
  // Setting BLUE (B) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  
  // Reading the output frequency
  blueFrequency = pulseIn(sensorOut, LOW);
  
  // Printing the BLUE (B) value 
  //Serial.print(" B = ");Serial.print("\t");
  Serial.print(blueFrequency);Serial.print("\t");
 // delay(100);

  Serial.print("E.Coli"); Serial.print("\t"); Serial.println(millis());
  digitalWrite(ecoli, LOW);   // turn the UV LED 

  delay(100);
  //Check Total Coliforms using white light
  digitalWrite(coliforms, HIGH);   // turn the white LED on 
  delay(100);
  
  // Setting RED (R) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  
  // Reading the output frequency
  redFrequency = pulseIn(sensorOut, LOW);
  
   // Printing the RED (R) value
  //Serial.print("R = ");Serial.print("\t");
  Serial.print(redFrequency);Serial.print("\t");
 // delay(100);
  
  // Setting GREEN (G) filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  
  // Reading the output frequency
  greenFrequency = pulseIn(sensorOut, LOW);
  
  // Printing the GREEN (G) value  
 // Serial.print(" G = ");Serial.print("\t");
  Serial.print(greenFrequency);Serial.print("\t");
 // delay(100);
 
  // Setting BLUE (B) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  
  // Reading the output frequency
  blueFrequency = pulseIn(sensorOut, LOW);
  
  // Printing the BLUE (B) value 
 // Serial.print(" B = ");Serial.print("\t");
  Serial.print(blueFrequency);Serial.print("\t");
 // delay(100);

  Serial.print("Coli"); Serial.print("\t"); Serial.println(millis());
  digitalWrite(coliforms, LOW);   // turn the UV LED 

  delay(60000);
  
}
