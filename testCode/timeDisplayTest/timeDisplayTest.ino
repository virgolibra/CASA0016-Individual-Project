// Test to display time by using TM1637 4 digit-display module.
// Dec 2021


#include <TM1637.h>
#include <ezTime.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h" 


char ssid[] = SECRET_SSID;        // your network SSID
char pass[] = SECRET_PASS;    // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;      //connection status
WiFiServer server(80);            //server socket

WiFiClient client = server.available();

//int ledPin = 2;

int CLK = 12;
int DIO = 13;

TM1637 tm(CLK,DIO);
Timezone GB;

void setup() {

    Serial.begin(9600);
//  pinMode(ledPin, OUTPUT);
  while (!Serial);
  
  enable_WiFi();
  connect_WiFi();

  server.begin();
  printWifiStatus();



  // put your setup code here, to run once:
  tm.init();

  //set brightness; 0-7
  tm.set(2);

   waitForSync();

  Serial.println("UTC: " + UTC.dateTime());

  GB.setLocation("Europe/London");
  Serial.println("London time: " + GB.dateTime());  

  displayNumber(8080);

}

void loop() {

    client = server.available();

  if (client) {
    printWEB();
  }
  
  // put your main code here, to run repeatedly:

//  // example: "12:ab"
//  // tm.display(position, character);
//  tm.display(0,1);
//  tm.display(1,2);
//  tm.point(1);
//  tm.display(2,10);
//  tm.display(3,11);
//
//  delay(1000);
//
//  // example: "1234"
//  displayNumber(1234);
//
//  delay(1000);
  Serial.println(GB.dateTime("H:i:s")); // UTC.dateTime("l, d-M-y H:i:s.v T")
  
//   if (minuteChanged()) {
//    
//   }
      Serial.println(hour());
    Serial.println(minute());
    displayHour();
    displayMinute();
    tm.point(1);

   

  
  delay(5000);
}

void displayNumber(int num){   
    tm.display(3, num % 10);   
    tm.display(2, num / 10 % 10);   
    tm.display(1, num / 100 % 10);   
    tm.display(0, num / 1000 % 10);
}

void displayHour(){
    int num = hour();  
    tm.display(1, num % 10);   
    tm.display(0, num / 10 % 10);   
}

void displayMinute(){
    int num = minute();  
    tm.display(3, num % 10);   
    tm.display(2, num / 10 % 10);   
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void enable_WiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

void printWEB() {

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
           
            //create the buttons
            client.print("Click <a href=\"/H\">here</a> turn the LED on<br>");
            client.print("Click <a href=\"/L\">here</a> turn the LED off<br><br>");
            
            int randomReading = analogRead(A1);
            client.print("Random reading from analog pin: ");
            client.print(randomReading);
           
            
            

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

//        if (currentLine.endsWith("GET /H")) {
//        digitalWrite(ledPin, HIGH);        
//        }
//        if (currentLine.endsWith("GET /L")) {
//        digitalWrite(ledPin, LOW);       
//        }

      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
