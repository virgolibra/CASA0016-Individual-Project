/*
integrateCode.ino

CASA0016 Individual Project
Minghao ZHANG
Dec 2021

*/
#include "arduino_secrets.h" 

#include <TM1637.h>   // 7-segment display
#include <WiFiNINA.h> // Wi-Fi connection
#include <ezTime.h>   // GB time
#include <Wire.h>
#include <LiquidCrystal.h> // lcd display
#include <MQ135.h>          // MQ135
#include <DHT.h>            // DHT
#include <Adafruit_BMP085.h> // BMP180

#include <ArduinoMqttClient.h>  // MQTT
#include <Adafruit_NeoPixel.h>

#define tmClkPin 12
#define tmDioPin 13
#define lcdRsPin 2
#define lcdEnPin 4
#define lcdD4Pin 8
#define lcdD5Pin 9
#define lcdD6Pin 10
#define lcdD7Pin 11
#define mq135Pin A1 // MQ135 Analog Input Pin
#define dhtPin 3 // DHT Digital Input Pin
#define buzzerPin 5 
#define soundPin 7
#define uvPin A3
#define pixelPin 6
#define btn1Pin 14  // A1
#define btn2Pin 16  // A2


#define DHTTYPE DHT11 // DHT11 or DHT22, depends on your sensor
#define seaLevelPressure_hPa 1013.25
#define NUMPIXELS 8
#define pixelBrightness 2
#define tmBrightness 0

// ------------------ Wi-Fi and MQTT connection ----------------------------------------------------
char ssid[] = SECRET_SSID;        // your network SSID
char pass[] = SECRET_PASS;    // your network password
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;

int keyIndex = 0;                 // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;      //connection status

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);


const char broker[] = "mqtt.cetools.org";
int        port     = 1884;
const char topic1_temperature[]  = "student/CASA0016/project/ucfnmz0/temperature";
const char topic2_humidity[]  = "student/CASA0016/project/ucfnmz0/humidity";
const char topic3_pressure[]  = "student/CASA0016/project/ucfnmz0/pressure";
const char topic4_altitude[]  = "student/CASA0016/project/ucfnmz0/altitude";
const char topic5_ppm[]  = "student/CASA0016/project/ucfnmz0/ppm";
const char topic6_sound[]  = "student/CASA0016/project/ucfnmz0/soundStatus";
const char topic7_uv[]  = "student/CASA0016/project/ucfnmz0/uvValue";

// -------------------------------------------


TM1637 tm(tmClkPin,tmDioPin);
Timezone GB;
Adafruit_BMP085 bmp;
LiquidCrystal lcd(lcdRsPin, lcdEnPin, lcdD4Pin, lcdD5Pin, lcdD6Pin, lcdD7Pin);
MQ135 mq135_sensor(mq135Pin);
DHT dht(dhtPin, DHTTYPE);
Adafruit_NeoPixel pixels(NUMPIXELS, pixelPin);

float bmpPressure = 0; // pressure and altitude (BMP180)
float bmpAltitude = 0; 
float dhtTemperature = 0; // Temp and humidity (DHT11)
float dhtHumidity = 0;
float mqPPM = 0;      // PPM (MQ135)
int soundDetectorStatus = 0;
int uvValue = 0;


boolean btn1_flag = false;
boolean muteFlag = false;

int btn1DisplayIndex = 0;



void setup() {

  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);

  // ------------- LCD init --------------------
  lcd.begin(16, 2);
  lcd.print("LCD init....");
  lcd.setCursor(0,1);
  lcd.print("successful");
  delay(500);
  //-------------- neoPixel ----------
  pixels.begin();
  setupPixelDisplay(1);
  // ------------- WIFI & MQTT init --------------------


  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);

  lcd.clear();
  lcd.print("Connecting to:");
  lcd.setCursor(0,1);
  lcd.print(ssid);
  lcd.setCursor(0,0);
  setupPixelDisplay(2);  
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  lcd.clear();
  lcd.print("Wi-Fi Connected");
  lcd.setCursor(0,1);
  lcd.print("IP:");
  lcd.setCursor(3,1);
  lcd.print(WiFi.localIP());
  setupPixelDisplay(3);
  delay(1000);

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  lcd.clear();
  lcd.print("Connect to MQTT");
  setupPixelDisplay(4);
  mqttClient.setUsernamePassword(mqttuser, mqttpass);
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  lcd.setCursor(0,1);
  lcd.print("Successful");
  setupPixelDisplay(5);
  delay(1000);

  lcd.clear();
  lcd.print("Setup......");
  
  // ------------- BMP 180 init --------------------
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP180 sensor, check wiring!");
    while (1);
  }

  // ------------- DHT init --------------------
  dht.begin();
  setupPixelDisplay(6);
  
  // ------------- Time display --------------------
  tm.init();
  tm.set(tmBrightness);  //set brightness; 0-7

  // Local Time GB
  waitForSync();
  Serial.println("UTC: " + UTC.dateTime());
  GB.setLocation("Europe/London");
  Serial.println("London time: " + GB.dateTime());  
  
  setupPixelDisplay(7);

  // ------------- Buzzer ------------------------------------
  pinMode(buzzerPin, OUTPUT); // Set buzzer - pin 9 as an output
  digitalWrite(buzzerPin, HIGH);

  // ------------- Sound Detector ---------------------------
  pinMode(soundPin, INPUT);
  // ------------- UV Sensor ---------
  pinMode(uvPin, INPUT);
  setupPixelDisplay(8);


  //-------------- Button & interrupt  ISR -----------------

  attachInterrupt(digitalPinToInterrupt(btn1Pin), btn1_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(btn2Pin), btn2_ISR, RISING);

  lcd.setCursor(0,1);
  lcd.print("Done");
  setupDonePixelDisplay();
  delay(1000);


  // -------------- init sensor data update --------
    displayTime();
    getBmpData();
    getDhtData();
    getMqData();
    getSoundData();
    getUvData();
    sendMQTT();
//    updateLcdDisplay();
//    updatePixelDisplay();
}

void loop() {

   mqttClient.poll();

  if (minuteChanged()) {
    displayTime();
    getBmpData();
    getDhtData();
    getMqData();
   
    getUvData();
    sendMQTT();


    updateNTP();
  }

  checkIfButtonSound();
   getSoundData();

  
  
}


void displayTime(){
  Serial.print(">>> Time Update | ");
  Serial.println(GB.dateTime("H:i:s")); // UTC.dateTime("l, d-M-y H:i:s.v T")
  
  // display hour
  int numHour = hour();  
  tm.display(1, numHour % 10);   
  tm.display(0, numHour / 10 % 10);

  // display minute 
  int numMinute = minute();  
  tm.display(3, numMinute % 10);   
  tm.display(2, numMinute / 10 % 10);    

  tm.point(1);


  if (muteFlag){
    tone(buzzerPin, 1000);
    delay(20);
    noTone(buzzerPin);

    digitalWrite(buzzerPin, HIGH);
  }

  
}

void getBmpData(){
  bmpPressure = bmp.readSealevelPressure()*0.01;
  bmpAltitude = bmp.readAltitude(seaLevelPressure_hPa * 100);

  Serial.print(">>> BMP Update |");
  Serial.print(" Pressure: ");
  Serial.print(bmpPressure);
  Serial.print(" hPa");
  Serial.print(" | Real altitude: ");
  Serial.print(bmpAltitude);
  Serial.println(" meters");

}

void getDhtData(){
  dhtHumidity = dht.readHumidity();
  dhtTemperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(dhtHumidity) || isnan(dhtTemperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(">>> DHT Update |");
  Serial.print(" Temperature: ");
  Serial.print(dhtTemperature);
  Serial.print(" C");
  Serial.print(" | Humidity: ");
  Serial.print(dhtHumidity);
  Serial.println("%");

}

void getMqData(){
  mqPPM = mq135_sensor.getCorrectedPPM(dhtTemperature, dhtHumidity);
  Serial.print(">>> MQ135 Update | ");
  Serial.print("PPM: ");
  Serial.println(mqPPM);
}

void getSoundData(){
  soundDetectorStatus = !digitalRead(soundPin);
//  Serial.print(">>> Sound Detector Update | ");
//  Serial.print("Status: ");
//  Serial.println(soundDetectorStatus);

  if (soundDetectorStatus){
    soundDetectPixelDisplay();
    mqttClient.beginMessage(topic6_sound);
    mqttClient.print(soundDetectorStatus);
    mqttClient.endMessage();

    lcd.clear();
    lcd.print("High volume");
    lcd.setCursor(0,1);
    lcd.print("Detected");
    
  }
}

void getUvData(){
  uvValue = analogRead(uvPin);
  Serial.print(">>> UV Sensor Update | ");
  Serial.print("UV Value: ");
  Serial.println(uvValue);
}

void updateLcdDisplay(){
  lcd.clear();
  lcd.print("To MQTT ");
  lcd.setCursor(0,1);
  lcd.print(GB.dateTime("d M H:i:s"));

}

void setupPixelDisplay(int numberOfPixelOn){
  pixels.clear();
  for(int i=0; i<numberOfPixelOn; i++) {
    pixels.setPixelColor(i, 150, 0, 0); // red
    pixels.setBrightness(pixelBrightness);
  }
  pixels.show();
}

void lcdInfoPixelDisplay(int indexOfPixelOn){
  pixels.clear();
  pixels.setPixelColor(indexOfPixelOn, 102, 0, 204); //purple
  pixels.setBrightness(pixelBrightness);
  pixels.show();
}

void mqttUploadPixelDisplay(){
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.clear();
    pixels.setPixelColor(i, 0, 102, 204); // blue
    pixels.setBrightness(pixelBrightness);
    pixels.show();
    delay(50);
  }

  pixels.clear();
  pixels.show();
  delay(100);
  
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, 0, 102, 204); // blue
    pixels.setBrightness(pixelBrightness);
  }
  pixels.show();
 
  
}


void soundDetectPixelDisplay(){
  pixels.clear();
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, 204, 102, 0); // orange 
    pixels.setBrightness(pixelBrightness);
  }
  pixels.show();
}


void setupDonePixelDisplay(){
  pixels.clear();
  for(int i=0; i<NUMPIXELS; i++) {
//    pixels.setPixelColor(i, 150, 0, 0); //red 
    pixels.setPixelColor(i, 0, 155, 0); //green 
    pixels.setBrightness(pixelBrightness);
    

  }

  pixels.show();
}

void checkIfButtonSound(){
    if (muteFlag && btn1_flag){
    tone(buzzerPin, 500);
    delay(20);
    noTone(buzzerPin);

    digitalWrite(buzzerPin, HIGH);
    btn1_flag = false;
  }
}

// ------------------------------------------------------------------------------------
//                                    Button ISR
// ------------------------------------------------------------------------------------
void btn1_ISR(){
  lcd.clear();

  switch(btn1DisplayIndex){
    case 0:
      lcd.print("CASA0016-Project");
      lcd.setCursor(0,1);
      lcd.print("Minghao");
      lcd.setCursor(11,1);
      lcd.print("(1/7)");
      lcdInfoPixelDisplay(1);
      btn1DisplayIndex++;
      break;
    case 1:
      lcd.print("Temperature");
      lcd.setCursor(0,1);
      lcd.print(dhtTemperature);
      lcd.setCursor(11,1);
      lcd.print("(2/7)");
      lcdInfoPixelDisplay(2);
      btn1DisplayIndex++;
      break;
    case 2:
      lcd.print("Humidity");
      lcd.setCursor(0,1);
      lcd.print(dhtHumidity);
      lcd.setCursor(11,1);
      lcd.print("(3/7)");
      lcdInfoPixelDisplay(3);
      btn1DisplayIndex++;
      break;
    case 3:
      lcd.print("Pressure");
      lcd.setCursor(0,1);
      lcd.print(bmpPressure);
      lcd.setCursor(11,1);
      lcd.print("(4/7)");
      lcdInfoPixelDisplay(4);
      btn1DisplayIndex++;
      break;      
    case 4:
      lcd.print("Altitude");
      lcd.setCursor(0,1);
      lcd.print(bmpAltitude);
      lcd.setCursor(11,1);
      lcd.print("(5/7)");
      lcdInfoPixelDisplay(5);
      btn1DisplayIndex++;
      break;
    case 5:
      lcd.print("PPM");
      lcd.setCursor(0,1);
      lcd.print(mqPPM);
      lcd.setCursor(11,1);
      lcd.print("(6/7)");
      lcdInfoPixelDisplay(6);
      btn1DisplayIndex++;
      break;  
    case 6:
      lcd.print("UV Value");
      lcd.setCursor(0,1);
      lcd.print(uvValue);
      btn1DisplayIndex = 0;
      lcd.setCursor(11,1);
      lcdInfoPixelDisplay(7);
      lcd.print("(7/7)");
      break;
    default:
      btn1DisplayIndex = 0;
      break;
      
  }

  btn1_flag = true;
}

void btn2_ISR(){
  lcd.clear();
  if (muteFlag){
    lcd.print("Slient Mode ON");
  }else
  {
    lcd.print("Slient Mode OFF");
  }

  muteFlag = !muteFlag;
}




// ------------------------------------------------------------------------------------
//                                    send MQTT
// ------------------------------------------------------------------------------------
void sendMQTT(){


  
    // call poll() regularly to allow the library to send MQTT keep alive which
  // avoids being disconnected by the broker
  mqttClient.poll();

  if(mqttClient.connected()){
    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic1_temperature);
    mqttClient.print(dhtTemperature);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic2_humidity);
    mqttClient.print(dhtHumidity);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic3_pressure);
    mqttClient.print(bmpPressure);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic4_altitude);
    mqttClient.print(bmpAltitude);
    mqttClient.endMessage();
  
    mqttClient.beginMessage(topic5_ppm);
    mqttClient.print(mqPPM);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic6_sound);
    mqttClient.print(soundDetectorStatus);
    mqttClient.endMessage();
  
    mqttClient.beginMessage(topic7_uv);
    mqttClient.print(uvValue);
    mqttClient.endMessage();
  
    Serial.println("Sending messages to MQTT done!");
    Serial.println();
    lcd.clear();
    lcd.print("To MQTT @");
    lcd.setCursor(10,0);
    lcd.print(GB.dateTime("H:i"));
    lcd.setCursor(0,1);
    lcd.print("CASA0016-Minghao");
    mqttUploadPixelDisplay();    
  }
   
  else{ // if the client has been disconnected, 
    Serial.println("Client disconnected, attempting reconnection");
    Serial.println();
    lcd.clear();
    lcd.print("MQTT Send Error");
    lcd.setCursor(0,1);
    lcd.print("Reconnect..");
    
    if(!attemptReconnect()){ // try reconnecting
      Serial.print("Client reconnected!");
      Serial.println();  
      lcd.setCursor(11,1);
      lcd.print("done");  
    }

  }

}


int attemptReconnect(){
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

  }
  return mqttClient.connectError(); // return status
}
