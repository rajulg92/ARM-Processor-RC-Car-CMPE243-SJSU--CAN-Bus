#include "Magnetometer.h"
//#include <Arduino.h>
#define RAD_TO_DEG 57.295779513082320876798154814105
Magnetometer::~Magnetometer()
{
}

double Magnetometer::radiansToDegrees(double radians)
{
    return radians * RAD_TO_DEG;
}

double Magnetometer::computeVectorAngle(int16_t y, int16_t x)
{
    double degrees = radiansToDegrees(-atan2(y, x));
#if 0
    if (degrees < 0)
    {
        degrees += 360.0;
    }
    if (degrees > 360.0)
    {
        degrees -= 360.0;
    }
#endif
#if 1
    if (degrees < 0)
    {
        degrees += 360.0;
    }
    if (degrees > 360.0)
    {
        degrees -= 360.0;
    }
#endif

    return degrees;
}
