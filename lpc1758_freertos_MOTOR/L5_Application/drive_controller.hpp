/*
 * drive_controller.hpp
 *
 *  Created on: Oct 14, 2017
 *      Author: unnik
 */

#ifndef DRIVE_CONTROLLER_HPP_
#define DRIVE_CONTROLLER_HPP_

#include <stdint.h>

#include "lpc_pwm.hpp"
#include "singleton_template.hpp"
#include "scheduler_task.hpp"

#define ESC_PWM_FREQ_HZ (50)
#define SERVO_PWM_FREQ_HZ (50)


#define ESC_NEUTRAL (7.5)
#define ESC_FULL_FWD (10.0)
#define ESC_FULL_REV (5.0)
#define ESC_REV_SLOW (7.0)

//Forward: slowest is 7.8; shall try 7.75; 7.7 does not work
#define ESC_FWD_SLOW (7.7)

#define SERVO_HARD_LEFT ESC_FULL_REV
#define SERVO_HARD_RIGHT ESC_FULL_FWD
#define SERVO_STRAIGHT ESC_NEUTRAL

typedef enum
{
    kDriveControllerCmd_Rev
}eDriveControllerCmd;

typedef struct
{
    eDriveControllerCmd cmd;
    float fSpeedPercent;
    float fSteerPercent; //not used now
}tDriveControllerCmd;

/**
 * Optimus DriveController class
 * @brief Controls the Servo and ESC (Electronic Speed Controller)
 *
 */
class DriveController : public scheduler_task, public SingletonTemplate<DriveController>
{
public:
    /**
     * @brief Drive Forward at a speed defined by the percent value passed
     * @param afSpeedPercent [IN] The speed percent; 0 - 100%
     * @return
     */
    bool escForward(float afSpeedPercent);
    bool escNeutral();
    bool escReverseTraxxasXL5(float afSpeedPercent);
    bool escReverse(float afSpeedPercent);

    bool servoRight(float afSteerRightPercent);
    bool servoStraight();
    bool servoLeft(float afSteerLeftPercent);

    float getDutyCycle(float afPercent, float afLowBoud, float afUpperBound, bool bTowardUpper);
    float getESCFwdPWMDutyCycle(float afSpeedPercent);
    float getESCRevPWMDutyCycle(float afSpeedPercent);
    float getServoRightPWMDutyCycle(float afSteerRightPercent);
    float getServoLeftPWMDutyCycle(float afSteerLeftPercent);

    /** scheduler_task functions: */
    bool init(void);
    bool run(void *param);
private:
    PWM esc;
    PWM servo;
    QueueHandle_t hCommandQueue;
    DriveController() :
            scheduler_task("DriveCtrlr", 512 * 8, PRIORITY_HIGH, NULL),
            esc(PWM(PWM::pwm2, ESC_PWM_FREQ_HZ)),
            servo(PWM(PWM::pwm1, SERVO_PWM_FREQ_HZ)),
            hCommandQueue(0)
    {
        //start the ESC in neutral and servo straight
        escNeutral();
        servoStraight();
    }
    friend class SingletonTemplate<DriveController>;
};




#endif /* DRIVE_CONTROLLER_HPP_ */
