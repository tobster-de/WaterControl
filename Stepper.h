// Stepper.h

#ifndef STEPPER_H
#define STEPPER_H

#include "Arduino.h"

typedef enum
{
    clockwise = 0, 
    counter_clockwise = 1,
} direction_t;

class Stepper
{
    uint8_t dirPin;
    uint8_t stepPin;
    double turnPerStep;

    void setDirection(direction_t direction) const;
    void step(int steps) const;
public:
    Stepper(uint8_t dirPin, uint8_t stepPin, uint16_t stepsPerRevolution, double gearRatio);

    double turn(double angle, direction_t direction);
    double getMinTurn() const;
};

#endif

