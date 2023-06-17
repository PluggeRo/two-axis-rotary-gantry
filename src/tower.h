/*
    Tower.h - Library for controlling a Tower run by 2 Stepper Motors
*/

#ifndef Tower_h
#define Tower_h

#include "Arduino.h"

class Tower
{
    public:
        Tower();
        void turn(int direction, float angle, int speed);
        void turnW(int direction, int& condition1, int speed);
        void tilt(int direction, int angle, int speed);
        void tiltW(int direction, int& condition2, int speed);
        void homeTurn();
        void moveToV(int AngleV, int speed);
        void moveToH(int AngleH, int speed);
        
    private:
        void angleToStep(int angleS);
        
        void refreshInputs();

};

#endif