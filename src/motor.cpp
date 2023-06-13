/*
    Motor.cpp - Library for controlling a Stepper Motor MKS Servo 42-C
*/

#include "Arduino.h"
#include "motor.h"



Motor::Motor(byte addr)
{
    motorAddr = addr;
    Serial1.begin(38400);
}

void Motor::stop()
{
    byte cmmd = 0xf7;
    byte tCHK = (motorAddr + cmmd) & 0xFF; //Checkbyte 
    byte signal[] = {motorAddr, cmmd,  tCHK};

    Serial1.write(signal, sizeof(signal));
}
void Motor::turn(int dir, int speed)
{
    Serial.print(" Adresse: ");
    Serial.print(motorAddr);
    byte cmmd = 0xf6;

    byte dirAndSpeed = 0x00;
    //Bestimmung der Richtung
    byte cclkw = 0x80;
    byte clkw = 0x00;

    if(dir == 0) 
    {
        dirAndSpeed = dirAndSpeed | cclkw;
    }
    else
    {
        dirAndSpeed = dirAndSpeed | clkw;
    }

    byte speedByte = speed;
    dirAndSpeed = dirAndSpeed | speedByte;

    byte tCHK = (motorAddr + cmmd + dirAndSpeed) & 0xFF; //Checkbyte 
    byte signal[] = {motorAddr, cmmd, dirAndSpeed, tCHK}; 

    Serial1.write(signal, sizeof(signal));

}
void Motor::turnSteps(int dir, int speed, long steps)
{
    byte dirAndSpeed = 0x00;
    //Bestimmung der Richtung
    byte cclkw = 0x80;
    byte clkw = 0x00;

    if(dir == 0) 
    {
        dirAndSpeed = dirAndSpeed | cclkw;
    }
    else
    {
        dirAndSpeed = dirAndSpeed | clkw;
    }

    byte speedByte = speed;
    dirAndSpeed = dirAndSpeed | speedByte;

    byte cmmd = 0xfd; //Kommando festlegen

    byte s1 = byte(steps>>24); //Number of Pulses part 1, höchstes Byte des Winkels, Bitshift um 24 Stellen 
    byte s2 = byte(steps>>16); //Number of Pulses part 2, zweit höchstes Byte des Winkels, Bitshift um 16 Stellen
    byte s3 = byte(steps>>8); //Number of Pulses part 3, zweit niedrigstes Byte des Winkels, Bitshift um 8 Stellen
    byte s4 = byte(steps); //Number of Pulses part 4, niedrigstes Byte des Winkels

    byte tCHK = (motorAddr + cmmd + dirAndSpeed + s1 + s2 + s3 + s4) & 0xFF; //Checkbyte 

    byte signal[] = {motorAddr, cmmd, dirAndSpeed, s1, s2, s3, s4, tCHK};

    Serial1.write(signal, sizeof(signal));
}

void Motor::setZero()
{
    byte cmmd = 0x90;
    byte tCHK = (motorAddr + cmmd + 0x01) & 0xFF; //Checkbyte 
    byte signalSetZeroMode[] = {motorAddr, cmmd, 0x00, tCHK};

    Serial1.write(signalSetZeroMode, sizeof(signalSetZeroMode));

    cmmd = 0x91;
    tCHK = (motorAddr + cmmd + 0x00) & 0xFF; //Checkbyte 
    byte signalSetZero[] = {motorAddr, cmmd, 0x00, tCHK};

    Serial1.write(signalSetZero, sizeof(signalSetZero));

    cmmd = 0x90;
    tCHK = (motorAddr + cmmd + 0x00) & 0xFF; //Checkbyte 
    byte signalResetZeroMode[] = {motorAddr, cmmd, 0x00, tCHK};

    Serial1.write(signalResetZeroMode, sizeof(signalResetZeroMode));
}