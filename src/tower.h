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
        void turn(int direction, float angle);
        void turnW(int direction, int& condition1);
        void tilt(int direction, int angle);
        void tiltW(int direction, int& condition2);
        void homeTurn();
        
    private:
        void angleToStep(int angleS);
        
        void refreshInputs();

};

#endif