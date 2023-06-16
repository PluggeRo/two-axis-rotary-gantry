/*
    Tower.cpp - Library for controlling a Tower run by 2 Stepper Motors
*/

#include "Arduino.h"
#include "tower.h"
#include "motor.h"


Motor motorH(0xe1); //Motor Drehung um horizontal Achse
Motor motorV(0xe0); //Motor Drehung um vertikale Achse

int incomingByte = Serial1.read();

//Zähnezahlen Zahnräder + Übersetzungverhältnis

//Abtrieb Drehung Turm
float zAbTurm = 104;
//Antrieb Drehung Turm
float zAnTurm = 40;
//Antrieb Mittelachse
float zAnMitte = 40;
//Abtrieb Mittelachse
float zAbMitte = 16;
//Schneckenrad
float zSchnecke = 1;
//Abtrtieb Schneckentrieb
float zAbSchnecke = 40;

//Übersetzungsverhältnis Drehung Turm
float iTurm = zAbTurm / zAnTurm;
//Übersetzungverhältnis Drehung Mittelachse
float iMitte = zAbMitte / zAnMitte;
//Übersetzungverhältnis Drehung Zeiger
float iZeiger = iMitte * (zAbSchnecke/zSchnecke);

//Schritte pro Winkel
float stpPerA = 8.89 * 4; //MStep gesetzt von 16 auf 64

//Endschalter Pins
const int pinEndschalterTurn = 22;  // Pin Nummer Endschalter Drehung um vertikal Achse
const int pinEndschalterTiltO = 26;  // Pin Nummer Endschalter Drehung um horizontal Achse Oben
const int pinEndschalterTiltU = 24;  // Pin Nummer Endschalter Drehung um horizontal Achse Unten

//Schalter Variablen
int StatusEndschalterTurn = 0;  // variable for reading the pushbutton status
int StatusEndschalterTiltO = 0;  // variable for reading the pushbutton status
int StatusEndschalterTiltU = 0;  // variable for reading the pushbutton status

//Vorbereitung Koordinaten
int winkelAchseH; //Absoluter Positionswinkel um die Horizontal Achse (Zeiger)
int winkelAchseV; // Absoluter Positionswinkel um die Vertikal Achse (Turm)

Tower::Tower()
{
    //Enable serial connection with motor
    Serial1.begin(38400);
    //Serial.begin(9600);

    pinMode(pinEndschalterTurn, INPUT);   
    //motorH.angle();
}

/// @brief Turm dreht um Vertikale Achse um angegebenen Winkel
/// @param direction CCKW = 1; CKW = 0; positive Richtung Winkel: CKW
/// @param angle 
/// @param speed 
void Tower::turn(int direction, float angle, int speed)
{
    int direction2;

    //Berechnung des Drehwinkels des Motors 1
    float winkelM1 = angle * iTurm;

    //Berechnung des Drehwinkels des Motors 2
    float winkelM2 = angle * iMitte;

    //Umrechnung des Drehwinkels in Schritte
    //Motor 1
    float save = winkelM1 * stpPerA;
    long stepsM1 = save; //Schritte als long gespeichert, speicher beansprucht 4 bytes, wie in der Eingabe gefordert 

    //Motor 2
    float save2 = winkelM2 * stpPerA;
    long stepsM2 = save2;

    if(direction == 0) 
    {
        direction2 = 1;
    }
    else
    {
        direction2 = 0;
    }

    //Bewegung Turm
    motorV.turnSteps(direction, speed, stepsM1);
    //Drehung Ausgleich Schneckenrad
    motorH.turnSteps(direction2, speed, stepsM2);
    

    // aktualisieren des absoluten Winkels um die Vertikale Achse Turm
    if(direction == 0)
    {
        winkelAchseV = winkelAchseV + angle;
    }
    else
    {
        winkelAchseV = winkelAchseV - angle;
    }
    
    delay(1000); // adjust delay based on your motor's speed

}

void Tower::turnW(int direction, int& condition1, int speed)
{
    refreshInputs();
    motorV.turn(direction, speed);
    while(condition1 == 0)
    {
        refreshInputs();
    } 
    motorV.stop();
    delay(1000);
}

/// @brief Zeiger dreht um horizontale Achse um angegebenen Winkel
/// @param direction hoch = 1; runter = 0; positive Richtung Winkel: runter
/// @param angle 
/// @param speed
void Tower::tilt(int direction, int angle, int speed)
{
    //Berechnung des Drehwinkels des Motors 2
    float winkelM2 = angle * iZeiger;

    //Umrechnung des Drehwinkels in Schritte
    //Motor 2
    float save2 = winkelM2 * stpPerA;
    long stepsM2 = save2;

    motorH.turnSteps(direction, speed, stepsM2);

    // aktualisieren des absoluten Winkels um die horizontale Achse Zeiger
    if(direction == 0)
    {
        winkelAchseH = winkelAchseH + angle;
    }
    else
    {
        winkelAchseH = winkelAchseH - angle;
    }

    delay(1000); // adjust delay based on your motor's speed
    
}

void Tower::tiltW(int direction, int& condition2, int speed)
{
    refreshInputs();
    motorH.turn(direction, speed);
    while(condition2 == 0)
    {
        refreshInputs();
    } 
    motorH.stop();
    delay(1000);
}

void Tower::angleToStep(int angleS)
{

}

/// @brief Der Turm nullt die Winkel an den Endschalter um den Winkel nur in positiver Richtung zu verfahren und fährt die Homeposition an
void Tower::homeTurn()
{
    delay(1000);
    turnW(1, StatusEndschalterTurn, 10);
    turn(0, 15, 10); //richtung muss 0 sein!!!
    turnW(1, StatusEndschalterTurn, 2);
    motorV.setZero();
    winkelAchseV = 0;
    tiltW(1, StatusEndschalterTiltO, 10);
    tilt(0, 10, 15);
    tiltW(1, StatusEndschalterTiltO, 2);
    motorH.setZero();
    winkelAchseH = 0;

    turn(0, 180, 15);
    tilt(0, 90, 15); 

    turn(1, 10, 15);
    tilt(1, 10, 15);

    delay(1000);
}

void Tower::refreshInputs()
{
    StatusEndschalterTurn = digitalRead(pinEndschalterTurn);
    StatusEndschalterTiltO = digitalRead(pinEndschalterTiltO);
    StatusEndschalterTiltU = digitalRead(pinEndschalterTiltU);
}

void Tower::moveTo(int AngleV, int AngleH, int speed)
{
    if(winkelAchseV > AngleV)
    {
        turn(1, winkelAchseV - AngleV, speed);
    }
    else if(winkelAchseV < AngleV)
    {
        turn(0, AngleV - winkelAchseV, speed);
    }

    if(winkelAchseH > AngleH)
    {
        tilt(1, winkelAchseH - AngleH, speed);
    }
    else if(winkelAchseH < AngleH)
    {
        tilt(0, AngleH - winkelAchseH, speed);
    }
}

