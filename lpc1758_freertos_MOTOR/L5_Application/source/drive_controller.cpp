/*
 * drive_controller.cpp
 * @brief The Drive (ESC + Servo) Controller implementation
 * with Optimus' glue code
 *
 *  Created on: Oct 14, 2017
 *      Author: unnik
 */

#include "drive_controller.hpp"
#include <stdio.h>
#include "tasks.hpp"
#include "queue.h"

//#define DEBUG_E
#include "debug_e.h"

bool DriveController::escForward(float afSpeedPercent)
{
    return esc.set(getESCFwdPWMDutyCycle(afSpeedPercent));
}

bool DriveController::escReverse(float afSpeedPercent)
{
    return esc.set(getESCRevPWMDutyCycle(afSpeedPercent));
}

bool DriveController::escNeutral()
{
    return esc.set(getESCFwdPWMDutyCycle(0.0));
}

bool DriveController::servoRight(float afSteerRightPercent)
{
    return servo.set(getServoRightPWMDutyCycle(afSteerRightPercent));
}

bool DriveController::servoStraight()
{
    return servo.set(getServoRightPWMDutyCycle(0.0));
}

bool DriveController::servoLeft(float afSteerLeftPercent)
{
    return servo.set(getServoLeftPWMDutyCycle(afSteerLeftPercent));
}

float DriveController::getDutyCycle(float afPercent, float afLowBound, float afUpperBound, bool bTowardUpper)
{
    //printf("afPercent=%f afLowBound=%f\n", afPercent, afLowBound);
    if(afPercent < 0.0
       || afPercent > 100.0
       || (afLowBound > afUpperBound))
    {
        return 0.0;
    }
    float fVal = ((afPercent/100.0) * (afUpperBound - afLowBound));
    return bTowardUpper ? (afLowBound + fVal) : (afUpperBound - fVal);
}

float DriveController::getESCFwdPWMDutyCycle(float afSpeedPercent)
{
    return getDutyCycle(afSpeedPercent, ESC_NEUTRAL, ESC_FULL_FWD, true);
}

float DriveController::getESCRevPWMDutyCycle(float afSpeedPercent)
{
    return getDutyCycle(afSpeedPercent, ESC_FULL_REV, ESC_NEUTRAL, false);
}

float DriveController::getServoRightPWMDutyCycle(float afSteerRightPercent)
{
    return getDutyCycle(afSteerRightPercent, SERVO_STRAIGHT, SERVO_HARD_RIGHT, true);
}

float DriveController::getServoLeftPWMDutyCycle(float afSteerLeftPercent)
{
    return getDutyCycle(afSteerLeftPercent, SERVO_HARD_LEFT, SERVO_STRAIGHT, false);
}

bool DriveController::init(void)
{
    hCommandQueue = xQueueCreate(1, sizeof(tDriveControllerCmd));
    LOGD("hCommandQ=%p\n", hCommandQueue);
    if(!hCommandQueue)
        return false;
    return true;
}

bool DriveController::escReverseTraxxasXL5(float afSpeedPercent)
{
    tDriveControllerCmd driveCmd;
    driveCmd.cmd = kDriveControllerCmd_Rev;
    driveCmd.fSpeedPercent = afSpeedPercent;
    LOGD("reverse %p\n", hCommandQueue);
    if(hCommandQueue)
    {
        return xQueueSend(hCommandQueue, &driveCmd, 0);
    }
    return false;
}

bool DriveController::run(void *param)
{
    tDriveControllerCmd driveCmd;
    LOGD("wait for command rev\n");
    if(xQueueReceive(hCommandQueue, &driveCmd, portMAX_DELAY) == pdTRUE)
    {
        LOGD("received cmd %d speed=%f\n", driveCmd.cmd, driveCmd.fSpeedPercent);
        escNeutral();
        vTaskDelay(100);
        escReverse(driveCmd.fSpeedPercent);
        vTaskDelay(100);
        escNeutral();
        vTaskDelay(100);
        escReverse(driveCmd.fSpeedPercent);
        vTaskDelay(100);
    }

    return true;
}
