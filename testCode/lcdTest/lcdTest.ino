
// This is a simple sketch to test lcd init and scroll characters
// Sketch to scroll characters onto an LCD screen

#include <LiquidCrystal.h>//See note in text about what is needed here, i.e., LiquidCrystal.h enclosed inside

// greater thanand less than symbols

// This site often removes greater than and less than symbols and the text between them

#define lcdRsPin 2
#define lcdEnPin 4
#define lcdD4Pin 8
#define lcdD5Pin 9
#define lcdD6Pin 10
#define lcdD7Pin 11

LiquidCrystal lcd(lcdRsPin, lcdEnPin, lcdD4Pin, lcdD5Pin, lcdD6Pin, lcdD7Pin);

// Declare lcd as a LiquidCrystal Object

char * messagePadded = "                This is the test to scroll characters onto the LCD screen.                ";

void setup()
{
  lcd.begin (16, 2);
  lcd.setCursor(3, 0);
  lcd.print("Hello World");
}

void loop()
{
  for (int letter = 0; letter <= strlen(messagePadded) - 16; letter++) //From 0 to upto n-16 characters supply to below function
  {
    showLetters(0, letter);
  }
}

void showLetters(int printStart, int startLetter)
{
  lcd.setCursor(printStart, 1);
  for (int letter = startLetter; letter <= startLetter + 15; letter++) // Print only 16 chars in Line #2 starting 'startLetter'
  {
    lcd.print(messagePadded[letter]);
  }
  lcd.print(" ");
  delay(500);
}
