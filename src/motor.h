/*
    Motor.h - Library for controlling a Stepper Motor MKS Servo 42-C
*/

#ifndef Motor_h
#define Motor_h

#include "Arduino.h"

class Motor
{
    public:
        Motor(byte addr);
        void stop();
        void turn(int dir, int speed);
        void turnSteps(int dir, int speed, long steps);
    private:
        byte motorAddr;

};

#endif