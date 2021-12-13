// buzzerTest.ino
// A simple sketch to test the buzzer
// Dec 2021


const int buzzerPin = 5; // set buzzer pin 5

void setup(){
 
  pinMode(buzzerPin, OUTPUT); // Set buzzer pin to output

}

void loop(){
 
  tone(buzzerPin,1000); // send 1 kHz sound signal 
  delay(20);
  noTone(buzzerPin);     // Stop sound...
  digitalWrite(buzzerPin, HIGH); // Stop sound
  delay(2000);
  
}
