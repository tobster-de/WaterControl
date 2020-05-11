/*
 * Stepper.cpp
 * Calculation of stepper movement and interfacing steppers
 * with a DRV8825 stepper driver
 */

#include "Stepper.h"

#define PRECISION       1000.0


void Stepper::step(int steps) const
{
    for (int i = 0; i < steps; i++)
    {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(800);

        digitalWrite(stepPin, LOW);
        delayMicroseconds(800);
    }
}

void Stepper::setDirection(direction_t direction) const
{
    digitalWrite(dirPin, (uint8_t)direction);
    delay(50);
}

Stepper::Stepper(uint8_t dirPin, uint8_t stepPin, uint16_t stepsPerRevolution, double gearRatio) :
    dirPin(dirPin),
    stepPin(stepPin)
{
    pinMode(dirPin, OUTPUT);
    digitalWrite(dirPin, LOW);

    pinMode(stepPin, OUTPUT);
    digitalWrite(stepPin, LOW);

    turnPerStep = 360.0 * gearRatio * PRECISION / stepsPerRevolution;
}

double Stepper::turn(double angle, direction_t direction)
{
    const int steps = trunc( angle * PRECISION / turnPerStep);
    const double possibleTurn = steps * turnPerStep / PRECISION;

    step(steps);

    return possibleTurn;
}

double Stepper::getMinTurn() const
{
    return turnPerStep;
}
