
const int button = 16; //buzzer to arduino pin 9


void setup(){
  Serial.begin(9600);
  while (!Serial);
  pinMode(button, INPUT); // Set buzzer - pin 9 as an output

}

void loop(){
 
 int statusBtn = digitalRead(button);
 Serial.print("status: ");
 Serial.println(statusBtn);
 delay(100);
  
}
