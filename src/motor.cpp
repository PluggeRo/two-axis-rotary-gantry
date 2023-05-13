/*
    Motor.cpp - Library for controlling a Stepper Motor MKS Servo 42-C
*/
//Penis

#include "Arduino.h"
#include "motor.h"

byte motorAddr;

Motor::Motor(byte addr)
{
    motorAddr = addr;
    Serial1.begin(38400);
}

void Motor::stop(int motorNr)
{
    byte cmmd = 0xf7;
    byte tCHK = (motorAddr + cmmd) & 0xFF; //Checkbyte 
    byte signal[] = {motorAddr, cmmd,  tCHK};

    Serial1.write(signal, sizeof(signal));
}
void Motor::turn(int motorNr, int dir, int speed)
{

}
void Motor::turnSteps(int motorNr, int dir, int speed, long steps)
{
    byte dirAndSpeed = 0x00;
    //Bestimmung der Richtung
    // direction = 1 ist gegen den Uhrzeigersinn
    byte cclkw = 0x80;
    // direction = 0 ist im Uhrzeigersinn
    byte clkw = 0x00;

    if(dir == 0) 
    {
        dirAndSpeed = dirAndSpeed | cclkw;
    }
    else
    {
        dirAndSpeed = dirAndSpeed | clkw;
    }

    int speedNr = 5; //geschwindigkeit festlegen
    byte speed = speedNr;
    dirAndSpeed = dirAndSpeed | speed;

    byte cmmd = 0xfd; //Kommande festlegen

    byte s1 = byte(steps>>24); //Number of Pulses part 1, höchstes Byte des Winkels, Bitshift um 24 Stellen 
    byte s2 = byte(steps>>16); //Number of Pulses part 2, zweit höchstes Byte des Winkels, Bitshift um 16 Stellen
    byte s3 = byte(steps>>8); //Number of Pulses part 3, zweit niedrigstes Byte des Winkels, Bitshift um 8 Stellen
    byte s4 = byte(steps); //Number of Pulses part 4, niedrigstes Byte des Winkels

    byte tCHK = (motorAddr + cmmd + dirAndSpeed + s1 + s2 + s3 + s4) & 0xFF; //Checkbyte 

    byte signal[] = {motorAddr, cmmd, dirAndSpeed, s1, s2, s3, s4, tCHK};

    Serial1.write(signal, sizeof(signal));
}