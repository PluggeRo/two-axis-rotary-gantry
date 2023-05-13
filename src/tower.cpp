/*
    Tower.cpp - Library for controlling a Tower run by 2 Stepper Motors
*/

#include "Arduino.h"
#include "tower.h"



int incomingByte = Serial1.read();
int control = 0xE1;

// Motor 1 = Drehung Turm um Vertikale Achse
// Motor 2 = Drehung Zeiger um Horizontale Achse
byte addrM1 = 0xe0; //Adresse Motor 1
byte addrM2 = 0xe1; //Adresse Motor 2

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
float stpPerA = 8.89;

//Stopsignal Motor 1
byte cmmdStop = 0xf7;
byte tCHKstopM1 = (addrM1 + cmmdStop) & 0xFF; //Checkbyte 
byte signalStopM1[] = {addrM1, cmmdStop, tCHKstopM1};

//Stopsignal Motor 2
byte tCHKstopM2 = (addrM2 + cmmdStop) & 0xFF; //Checkbyte 
byte signalStopM2[] = {addrM2, cmmdStop, tCHKstopM2};

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

    pinMode(pinEndschalterTurn, INPUT);   
}

void Tower::turn(int direction, float angle)
{
    byte dirAndSpeedM1 = 0x00;
    byte dirAndSpeedM2 = 0x00;
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

    //Bestimmung der Richtung
    // direction = 1 ist gegen den Uhrzeigersinn: Bit = 0
    byte cclkw = 0x80;
    // direction = 0 ist im Uhrzeigersinn: Bit = 1
    byte clkw = 0x00;

    if(direction == 0) 
    {
        dirAndSpeedM1 = dirAndSpeedM1 | cclkw;
        dirAndSpeedM2 = dirAndSpeedM2 | clkw;
    }
    else
    {
        dirAndSpeedM1 = dirAndSpeedM1 | clkw;
        dirAndSpeedM2 = dirAndSpeedM2 | cclkw;
    }

    //Einstellen der verfahrgeschwindigkeit
    int speedNr = 5;
    byte speed = speedNr;
    dirAndSpeedM1 = dirAndSpeedM1 | speed;

    dirAndSpeedM2 = dirAndSpeedM2 | speed;

    //Vorbereitung der Nachricht an Motor 1
    byte cmmdM1 = 0xfd; //Command ID 
    byte s1M1 = byte(stepsM1>>24); //Number of Pulses part 1, höchstes Byte des Winkels, Bitshift um 24 Stellen 
    byte s2M1 = byte(stepsM1>>16); //Number of Pulses part 2, zweit höchstes Byte des Winkels, Bitshift um 16 Stellen
    byte s3M1 = byte(stepsM1>>8); //Number of Pulses part 3, zweit niedrigstes Byte des Winkels, Bitshift um 8 Stellen
    byte s4M1 = byte(stepsM1); //Number of Pulses part 4, niedrigstes Byte des Winkels
    byte tCHKM1 = (addrM1 + cmmdM1 + dirAndSpeedM1 + s1M1 + s2M1 + s3M1 + s4M1) & 0xFF; //Checkbyte 
    byte signal1[] = {addrM1, cmmdM1, dirAndSpeedM1, s1M1, s2M1, s3M1, s4M1, tCHKM1};  

    //Vorbereitung der Nachricht an Motor 2
    //byte cmmdM1 = 0xfd; //Command ID 
    byte s1M2 = byte(stepsM2>>24); //Number of Pulses part 1, höchstes Byte des Winkels, Bitshift um 24 Stellen 
    byte s2M2 = byte(stepsM2>>16); //Number of Pulses part 2, zweit höchstes Byte des Winkels, Bitshift um 16 Stellen
    byte s3M2 = byte(stepsM2>>8); //Number of Pulses part 3, zweit niedrigstes Byte des Winkels, Bitshift um 8 Stellen
    byte s4M2 = byte(stepsM2); //Number of Pulses part 4, niedrigstes Byte des Winkels
    byte tCHKM2 = (addrM2 + cmmdM1 + dirAndSpeedM2 + s1M2 + s2M2 + s3M2 + s4M2) & 0xFF; //Checkbyte 
    byte signal2[] = {addrM2, cmmdM1, dirAndSpeedM2, s1M2, s2M2, s3M2, s4M2, tCHKM2};

    //Übertragung der Nachricht an den Motor
    Serial1.write(signal1, sizeof(signal1));
    Serial1.write(signal2, sizeof(signal2));
    
    //Serial1.write(signaltest, sizeof(signaltest));
    delay(1000); // adjust delay based on your motor's speed

}

void Tower::turnW(int direction, int& condition1)
{
    byte cmmdM1 = 0xf6;
    byte dirAndSpeedM1 = 0x00;
    byte dirAndSpeedM2 = 0x00;

    //Bestimmung der Richtung
    // direction = 1 ist gegen den Uhrzeigersinn: Bit = 0
    byte cclkw = 0x80;
    // direction = 0 ist im Uhrzeigersinn: Bit = 1
    byte clkw = 0x00;

    if(direction == 1) 
    {
        dirAndSpeedM1 = dirAndSpeedM1 | cclkw;
        dirAndSpeedM2 = dirAndSpeedM2 | clkw;
    }
    else
    {
        dirAndSpeedM1 = dirAndSpeedM1 | clkw;
        dirAndSpeedM2 = dirAndSpeedM2 | cclkw;
    }

    //Einstellen der verfahrgeschwindigkeit
    int speedNr = 1;
    byte speed = speedNr;
    dirAndSpeedM1 = dirAndSpeedM1 | speed;

    dirAndSpeedM2 = dirAndSpeedM2 | speed;
    byte tCHKTurn = (addrM1 + cmmdM1 + dirAndSpeedM1) & 0xFF; //Checkbyte 
    byte signalTurn[] = {addrM1, cmmdM1, dirAndSpeedM1, tCHKTurn}; 

    
    Serial.print("Test");
    Serial1.write(signalTurn, sizeof(signalTurn));
    Serial.print(condition1);
    Serial.print("Start");

    while(condition1 == 0)
    {

        Serial.print(condition1);
        refreshInputs();

    } 

    Serial.print(" ende");
    Serial1.write(signalStopM1, sizeof(signalStopM1));

}

void Tower::tilt(int direction, int angle)
{
    byte dirAndSpeedM2 = 0x00;

    //Berechnung des Drehwinkels des Motors 2
    float winkelM2 = angle * iZeiger;

    //Umrechnung des Drehwinkels in Schritte
    //Motor 2
    float save2 = winkelM2 * stpPerA;
    long stepsM2 = save2;

    //Bestimmung der Richtung
    // direction = 1 ist hoch: Bit = 0
    byte up = 0x00;
    // direction = 0 ist runter: Bit = 2
    byte down = 0x80;

    if(direction == 1) 
    {
        dirAndSpeedM2 = dirAndSpeedM2 | up;
    }
    else
    {
        dirAndSpeedM2 = dirAndSpeedM2 | down;
    }

    //Einstellen der verfahrgeschwindigkeit
    int speedNr = 3;
    byte speed = speedNr;

    dirAndSpeedM2 = dirAndSpeedM2 | speed;


    //Vorbereitung der Nachricht an Motor 2
    byte cmmdM1 = 0xfd; //Command ID 
    byte s1M2 = byte(stepsM2>>24); //Number of Pulses part 1, höchstes Byte des Winkels, Bitshift um 24 Stellen 
    byte s2M2 = byte(stepsM2>>16); //Number of Pulses part 2, zweit höchstes Byte des Winkels, Bitshift um 16 Stellen
    byte s3M2 = byte(stepsM2>>8); //Number of Pulses part 3, zweit niedrigstes Byte des Winkels, Bitshift um 8 Stellen
    byte s4M2 = byte(stepsM2); //Number of Pulses part 4, niedrigstes Byte des Winkels
    byte tCHKM2 = (addrM2 + cmmdM1 + dirAndSpeedM2 + s1M2 + s2M2 + s3M2 + s4M2) & 0xFF; //Checkbyte 
    byte signal2[] = {addrM2, cmmdM1, dirAndSpeedM2, s1M2, s2M2, s3M2, s4M2, tCHKM2};

    //Übertragung der Nachricht an den Motor
    Serial1.write(signal2, sizeof(signal2));
    
    //Serial1.write(signaltest, sizeof(signaltest));
    delay(1000); // adjust delay based on your motor's speed
}

void Tower::tiltW(int direction, int& condition2)
{
    byte cmmdM2 = 0xf6;
    byte dirAndSpeedM1 = 0x00;
    byte dirAndSpeedM2 = 0x00;

    //Bestimmung der Richtung
    // direction = 1 ist gegen den Uhrzeigersinn: Bit = 0
    byte cclkw = 0x80;
    // direction = 0 ist im Uhrzeigersinn: Bit = 1
    byte clkw = 0x00;

    if(direction == 1) 
    {
        dirAndSpeedM1 = dirAndSpeedM1 | cclkw;
        dirAndSpeedM2 = dirAndSpeedM2 | clkw;
    }
    else
    {
        dirAndSpeedM1 = dirAndSpeedM1 | clkw;
        dirAndSpeedM2 = dirAndSpeedM2 | cclkw;
    }

    //Einstellen der verfahrgeschwindigkeit
    int speedNr = 3;
    byte speed = speedNr;
    dirAndSpeedM1 = dirAndSpeedM1 | speed;

    dirAndSpeedM2 = dirAndSpeedM2 | speed;
    byte tCHKTilt = (addrM2 + cmmdM2 + dirAndSpeedM1) & 0xFF; //Checkbyte 
    byte signalTurn[] = {addrM2, cmmdM2, dirAndSpeedM1, tCHKTilt}; 

    
    Serial.print("Test");
    Serial1.write(signalTurn, sizeof(signalTurn));
    Serial.print(condition2);
    Serial.print("Start");

    while(condition2 == 0)
    {

        Serial.print(condition2);
        refreshInputs();

    } 

    Serial.print(" ende");
    Serial1.write(signalStopM2, sizeof(signalStopM2));
}

void Tower::angleToStep(int angleS)
{

}

void Tower::homeTurn()
{
    turnW(1, StatusEndschalterTurn);
    tiltW(0, StatusEndschalterTiltO);
    turn(1, 180);
    tilt(0, 90);
}

void Tower::refreshInputs()
{
    StatusEndschalterTurn = digitalRead(pinEndschalterTurn);
    StatusEndschalterTiltO = digitalRead(pinEndschalterTiltO);
    StatusEndschalterTiltU = digitalRead(pinEndschalterTiltU);
}

