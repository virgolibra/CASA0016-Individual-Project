// mqttTest.ino
// Contains two parts of codes. One for publish and another for subscribe
// Dec 2021


#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h" 


char ssid[] = SECRET_SSID;        // your network SSID
char pass[] = SECRET_PASS;    // your network password
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;



WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "mqtt.cetools.org";
int        port     = 1884;
const char topic[]  = "student/CASA0016/plant/ucfnmz0/real_unique_topic";
const char topic2[]  = "student/CASA0016/plant/ucfnmz0/real_unique_topic_2";
const char topic3[]  = "student/CASA0016/plant/ucfnmz0/real_unique_topic_3";

//set interval for sending messages (milliseconds)
const long interval = 8000;
unsigned long previousMillis = 0;

int count = 0;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

    // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  // mqttClient.setId("clientId");

  // You can provide a username and password for authentication
  // mqttClient.setUsernamePassword("username", "password");


  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

    mqttClient.setUsernamePassword(mqttuser, mqttpass);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alive which
  // avoids being disconnected by the broker
  mqttClient.poll();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    //record random value from A0, A1 and A2
    int Rvalue = analogRead(A0);
    int Rvalue2 = analogRead(A1);
    int Rvalue3 = analogRead(A2);

    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    Serial.println(Rvalue);

    Serial.print("Sending message to topic: ");
    Serial.println(topic2);
    Serial.println(Rvalue2);

    Serial.print("Sending message to topic: ");
    Serial.println(topic2);
    Serial.println(Rvalue3);

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    mqttClient.print(Rvalue);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic2);
    mqttClient.print(Rvalue2);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic3);
    mqttClient.print(Rvalue3);
    mqttClient.endMessage();

    Serial.println();
  }
}

// --- Subscribe------------------------------------------------------------------------------------

//#include <ArduinoMqttClient.h>
//#include <WiFiNINA.h>
//#include "arduino_secrets.h" 
//
//
//char ssid[] = SECRET_SSID;        // your network SSID
//char pass[] = SECRET_PASS;    // your network password
//const char* mqttuser = SECRET_MQTTUSER;
//const char* mqttpass = SECRET_MQTTPASS;
//
//
//
//WiFiClient wifiClient;
//MqttClient mqttClient(wifiClient);
//
//const char broker[] = "mqtt.cetools.org";
//int        port     = 1884;
//const char topic[]  = "UCL/PSW/Garden/WST/dvp2/windSpeed_kph";
//const char topic2[]  = "UCL/PSW/Garden/WST/dvp2/windDir";
//const char topic3[]  = "UCL/PSW/Garden/WST/dvp2/outHumidity";
//
//void setup() {
//  //Initialize serial and wait for port to open:
//  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }
//  // attempt to connect to Wifi network:
//  Serial.print("Attempting to connect to SSID: ");
//  Serial.println(ssid);
//  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
//    // failed, retry
//    Serial.print(".");
//    delay(5000);
//  }
//
//  Serial.println("You're connected to the network");
//  Serial.println();
//
//  Serial.print("Attempting to connect to the MQTT broker: ");
//  Serial.println(broker);
//
//  mqttClient.setUsernamePassword(mqttuser, mqttpass);
//
//  if (!mqttClient.connect(broker, port)) {
//    Serial.print("MQTT connection failed! Error code = ");
//    Serial.println(mqttClient.connectError());
//
//    while (1);
//  }
//
//  Serial.println("You're connected to the MQTT broker!");
//  Serial.println();
//
//  // set the message receive callback
//  mqttClient.onMessage(onMqttMessage);
//
//  Serial.print("Subscribing to topic: ");
//  Serial.println(topic);
//  Serial.println();
//
//  // subscribe to a topic
//  mqttClient.subscribe(topic);
//  mqttClient.subscribe(topic2);
//  mqttClient.subscribe(topic3);
//
//  // topics can be unsubscribed using:
//  // mqttClient.unsubscribe(topic);
//
//  Serial.print("Topic: ");
//  Serial.println(topic);
//  Serial.print("Topic: ");
//  Serial.println(topic2);
//  Serial.print("Topic: ");
//  Serial.println(topic3);
//
//  Serial.println();
//}
//
//void loop() {
//  // call poll() regularly to allow the library to receive MQTT messages and
//  // send MQTT keep alive which avoids being disconnected by the broker
//  mqttClient.poll();
//}
//
//void onMqttMessage(int messageSize) {
//  // we received a message, print out the topic and contents
//  Serial.println("Received a message with topic '");
//  Serial.print(mqttClient.messageTopic());
//  Serial.print("', length ");
//  Serial.print(messageSize);
//  Serial.println(" bytes:");
//
//  // use the Stream interface to print the contents
//  while (mqttClient.available()) {
//    Serial.print((char)mqttClient.read());
//  }
//  Serial.println();
//  Serial.println();
//}
