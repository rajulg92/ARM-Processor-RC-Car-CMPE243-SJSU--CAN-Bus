//#include "foo.hpp"
//#include "turtle.hpp"
//#include "sensor_val.hpp"
//#include "getpwm_motor.hpp"
#include "sensorVal.hpp"
#include "getPwmmotor.hpp"

int Pwmmotor::getMotorPWM(int x)
{
	//Turtle turtle;
	//return x + turtle.turtle_draw();
    SensorVal sensorVal;
    return sensorVal.readSensorValue(x);
  //  return 1;
}
