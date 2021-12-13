// Test code for BMP180 module
// Dec 2021

#include <Wire.h>
#include <Adafruit_BMP085.h>
#define seaLevelPressure_hPa 1013.25  // Pressure at sea level

Adafruit_BMP085 bmp;
  
void setup() {
  Serial.begin(9600);

  // BMP180 init and check if BMP180 exist
  if (!bmp.begin()) {
  Serial.println("Cannot find BMP180 Module");
  while (1) {}
  }
}
  
void loop() {
    Serial.print("Temperature: ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
    
    Serial.print("Pressure: ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");

    Serial.print("Altitude: ");
    Serial.print(bmp.readAltitude());
    Serial.println(" m");

    Serial.print("Calculated Pressure (Sea level): ");
    Serial.print(bmp.readSealevelPressure()*0.01);
    Serial.println(" hPa");

    Serial.print("Calculated Altitude: ");
    Serial.print(bmp.readAltitude(seaLevelPressure_hPa * 100));
    Serial.println(" m");
    
    Serial.println();
    delay(1000);
}
