#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

// Include for C++
using namespace cgreen;

// The file under test
#include "motor.hpp"

// Files we need to include
#include "sensor.hpp"



int sensor::readSensorValue(void) { return mock(); }

motor m;

Ensure(testing_call_to_motor_cond1_1)
{
    expect(readSensorValue, will_return(-1));
    assert_equal(m.getMotorPWM(), 10);
}
Ensure(testing_call_to_motor_cond1_2)
{
    expect(readSensorValue, will_return(101));
    assert_equal(m.getMotorPWM(), 10);
}

Ensure(testing_call_to_motor_cond2_1)
{
    expect(readSensorValue, will_return(0));
    assert_equal(m.getMotorPWM(), 10);
}

Ensure(testing_call_to_motor_cond2_2)
{
    expect(readSensorValue, will_return(49));
    assert_equal(m.getMotorPWM(), 10);
}

Ensure(testing_call_to_motor_cond3_1)
{
    expect(readSensorValue, will_return(50));
    assert_equal(m.getMotorPWM(), 10);
}

Ensure(testing_call_to_motor_cond3_2)
{
    expect(readSensorValue, will_return(100));
    assert_equal(m.getMotorPWM(), 10);
}
TestSuite *Motor_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, testing_call_to_motor_cond1_1);
    add_test(suite, testing_call_to_motor_cond1_2);
    add_test(suite, testing_call_to_motor_cond2_1);
    add_test(suite, testing_call_to_motor_cond2_2);
    add_test(suite, testing_call_to_motor_cond3_1);
    add_test(suite, testing_call_to_motor_cond3_2);
    return suite;
}
