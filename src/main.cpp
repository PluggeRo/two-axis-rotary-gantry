#include <tower.h>


Tower tower;
//const int buttonPin1 = 52;  // the number of the pushbutton pin cclkw
//const int buttonPin2 = 50;  // the number of the pushbutton pin clkw
//const int buttonPin3 = 48;  // the number of the pushbutton pin up
//const int buttonPin4 = 46;  // the number of the pushbutton pin down

// variables will change:
//int buttonState1 = 0;  // variable for reading the pushbutton status cclkw
//int buttonState2 = 0;  // variable for reading the pushbutton status clkw
//int buttonState3 = 0;  // variable for reading the pushbutton status up
//int buttonState4 = 0;  // variable for reading the pushbutton status down


void setup() {
  //Kommunikation mit Rechner
  Serial.begin(9600);

  //tower.turn(01, 180);
  //tower.tilt(00, 90);
  tower.homeTurn();
  delay(5000);
  tower.moveTo(45, 45, 15);
}

void loop() {
  /* 
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);
  buttonState3 = digitalRead(buttonPin3);
  buttonState4 = digitalRead(buttonPin4);

  if(buttonState1 == HIGH)
  {
    tower.turn(00, 20);
  }
  if(buttonState2 == HIGH)
  {
    tower.turn(01, 20);
  }
  if(buttonState3 == HIGH)
  {
    tower.tilt(01, 10);
  }
  if(buttonState4 == HIGH)
  {
    tower.tilt(00, 10);
  }
  */


}