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


Tower::Tower()
{
    //Enable serial connection with motor
    Serial1.begin(38400);
    //Serial.begin(9600);

    pinMode(pinEndschalterTurn, INPUT);   
    motorH.angle();
}

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

    motorV.turnSteps(direction, speed, stepsM1);

    motorH.turnSteps(direction2, speed, stepsM2);
    
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

void Tower::tilt(int direction, int angle, int speed)
{
    //Berechnung des Drehwinkels des Motors 2
    float winkelM2 = angle * iZeiger;

    //Umrechnung des Drehwinkels in Schritte
    //Motor 2
    float save2 = winkelM2 * stpPerA;
    long stepsM2 = save2;

    motorH.turnSteps(direction, speed, stepsM2);

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

void Tower::homeTurn()
{
    turnW(0, StatusEndschalterTurn, 10);
    turn(1, 45, 10);
    turnW(0, StatusEndschalterTurn, 2);
    tiltW(1, StatusEndschalterTiltO, 10);
    tilt(0, 10, 15);
    tiltW(1, StatusEndschalterTiltO, 2);

    turn(1, 180, 15);
    tilt(0, 90, 15);
    motorH.setZero();
    motorV.setZero();
    
}

void Tower::refreshInputs()
{
    StatusEndschalterTurn = digitalRead(pinEndschalterTurn);
    StatusEndschalterTiltO = digitalRead(pinEndschalterTiltO);
    StatusEndschalterTiltU = digitalRead(pinEndschalterTiltU);
}

