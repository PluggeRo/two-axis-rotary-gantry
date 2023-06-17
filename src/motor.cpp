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
    byte* response = readSignal(3);
    delete[] response;
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
    byte* response = readSignal(3);
    delete[] response;

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
    
    byte* responseBegin = readSignal(3);
    delete[] responseBegin;
    while(Serial1.available()<3)
    {
        //delay(10);
    }
    byte* responseEnd = readSignal(3);
    delete[] responseEnd;
    
}

void Motor::setZero()
{
   
    //Set ZeroMode to DirMode
    byte cmmd = 0x90;
    byte zeroModeEn = 0x01;
    byte tCHK = (motorAddr + cmmd + zeroModeEn) & 0xFF; //Checkbyte 
    byte signalSetZeroMode[] = {motorAddr, cmmd, zeroModeEn, tCHK};

    Serial1.write(signalSetZeroMode, sizeof(signalSetZeroMode));
    byte* setModeZero = readSignal(3);
    delete[] setModeZero;
    
    //Set Zero of ZeroMode
    byte cmmd2 = 0x91;
    byte zero = 0x00;
    byte tCHK2 = (motorAddr + cmmd2 + zero) & 0xFF; //Checkbyte 
    byte signalSetZero[] = {motorAddr, cmmd2, zero, tCHK2};

    Serial1.write(signalSetZero, sizeof(signalSetZero));
    //Nachricht wird nur empfangen, wenn der 0Mode manuell am motor gesetzt wird
    byte* setZeroByte = readSignal(3);
    delete[] setZeroByte;

    //Set ZeroMode to Disable
    byte zeroModeDis = 0x00;
    tCHK = (motorAddr + cmmd + zeroModeDis) & 0xFF; //Checkbyte 
    byte signalResetZeroMode[] = {motorAddr, cmmd, zeroModeDis, tCHK};

    Serial1.write(signalResetZeroMode, sizeof(signalResetZeroMode));
    byte* resetModeZero = readSignal(3);
    delete[] resetModeZero;

}
byte* Motor::readSignal(int expectedBytes)
{
    expectedBytes = expectedBytes + 1; // Der Motor gibt zur Trennung von Signalen immer noch ein Nullbyte mit
    byte* message = new byte[expectedBytes]; // Array to store the expected byte response
    byte* error = new byte[0];
    // Timeout variables for reading
    unsigned long startTime = millis();
    unsigned long timeout = 1000; // 1 second timeout

    int receivedBytes = 0;

    // Keep reading until we've received expected bytes or timeout has passed
    while (receivedBytes < expectedBytes && millis() - startTime < timeout) {
        if (Serial1.available()) {
            message[receivedBytes] = Serial1.read();
            receivedBytes++;
            //Serial.println("gelesen");
        }
    }

    // Handle timeout
    if (receivedBytes < expectedBytes) {
        //Serial.println("Failed to receive expected byte response");
        return error; // return error code or handle as appropriate
    }

    /*
    Serial.println("neue Lesefunktion: ");
    for(int i = 0; i < expectedBytes; i++)
    {
        Serial.println(message[i]);
    }
    */

    return message;
}
double Motor::angle()
{
    byte cmmd = 0x36;
    byte tCHK = (motorAddr + cmmd) & 0xFF; //Checkbyte 
    byte signalSetZeroMode[] = {motorAddr, cmmd, tCHK};

    Serial1.write(signalSetZeroMode, sizeof(signalSetZeroMode));
    byte* response = readSignal(6);

    uint32_t angleValue = ((uint32_t)response[1] << 24) |
                          ((uint32_t)response[2] << 16) |
                          ((uint32_t)response[3] << 8) |
                          (uint32_t)response[4];

    // Convert to degree: We assume the angle values go from 0 to 65535 for a full circle (360 degrees)
    double angleDegree = (angleValue / 65535.0) * 360;  
    delete[] response;

    Serial.println("Motorwinkel: ");
    Serial.print(angleDegree);
    return angleDegree;
}