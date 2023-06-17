#include <tower.h>


Tower tower;

char message[] = {'0','0','0','0','0','0','0','0'};
String signalRef = "G28";
String signalVerfVW = "G11";
String signalVerfVK = "G12";
String signalVerfHW = "G21";
String signalVerfHK = "G22";

int i = 0;
int angle = 0;
int direction = 0;

void setup() {
  //Kommunikation mit Rechner
  Serial.begin(9600);
  Serial.flush();

  //tower.homeTurn();
  delay(5000);
  //tower.moveTo(45, 45, 15);
}

void loop() 
{
  i = 0;
  while(Serial.available() > 0 && i<8)
  {
    message[i] = Serial.read();
    i++;
  }
  Serial.flush();
  if(message[0] == 'G')
  {
    for(int x=0; x<=7; x++)
    {
      Serial.println(message[x]);
      Serial.println('t');
    }
    tower.homeTurn();
    message[0] = '0';
  }

  Serial.flush();
  /*
  //verarbeiten der aufgenommenen Nachricht
  
  if(message[0] = 'G') //Beginnt Nachricht mit G ansonsten nichts
  {
    //übergebener Winkel auslesen
    for(int x = 5; x <= 7; x++ )
    {
      angle = (message[x] - '0') + (angle * 10);
    }
    switch(message[1]) //zweite Stelle der Nachricht überprüfen
    {
      case '1': // message[1] = 1 vertikales verfahren
        switch(message[2])// Überprüfung der Art des vertikalen verfahrens
        {
          case '1': //message[2] = 1 inkremental verfahren nach Winkel
            //bestimmung der Richtung
            if(message[4] == '-')
            {
              direction = 1;
            }
            else
            {
              direction = 0;
            }
            tower.turn(direction, angle, 15);

            break;
          case '2': //message[2] = 2 absolut verfahren zu Winkel Koordinate
            break;
          default:

        }

        break;
      case '2':// message[1] = 2 horizontales verfahren oder Referenzfahrt
        switch(message[2])// Überprüfung der Art des horizontalen verfahrens oder Referenzfahrt
        {
          case '1': //message[2] = 1 inkremental verfahren nach Winkel
            //bestimmung der Richtung
            if(message[4] == '-')
            {
              direction = 1;
            }
            else
            {
              direction = 0;
            }
            tower.tilt(direction, angle, 15);

            break;
          case '2': //message[2] = 2 absolut verfahren zu Winkel Koordinate
            break;

          case '8': //message[2] = 8 Referenzfahrt
            tower.homeTurn;
            break;
          default:

        }

        break;
      default:

    }
  }
  */
}