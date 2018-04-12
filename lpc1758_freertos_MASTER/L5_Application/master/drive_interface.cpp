/****************************************************************************
 * drive_interface.cpp
 * @file :
 * @created : Oct 16,2017
 *
 ****************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include "../../_can_dbc/generated_can.h"
#include <master/drive_interface.hpp>

//#define VERBOSE_E
//#define DEBUG_E
#include "debug.h"

/*Macros*/
#define MAX_STEER_PERCENTAGE 70

/* Method definition*/
IDriveController::IDriveController(): mcurr_speed(stop),mcurr_steer(steer_straight),
                                      mtarget_speed(speed_slow),mtarget_steer(steer_straight),mmoving_forward(true),
                                      mgeo_turning_angle(0.0),mnReverseSpeedOffset(12) //reverse minimum speed is min_fwd+3
{
    LOGD("IDriveController Object Instantiated");
    SpeedSteerMsg={0};

}

IDriveController::speed_mode_t IDriveController::get_current_speed()
{
    return mcurr_speed;
}

IDriveController::steer_dir_t IDriveController::get_current_steer()
{

    return mcurr_steer;
}

IDriveController::speed_mode_t IDriveController::get_target_speed()
{
   return mtarget_speed;
}
IDriveController::steer_dir_t IDriveController::get_target_steer()
{

    return mtarget_steer;
}

void IDriveController::set_target_speed(IDriveController::speed_mode_t speed,bool isfwd)
{
    mtarget_speed =speed;
    mmoving_forward = isfwd;
}

void IDriveController::set_target_steer(IDriveController::steer_dir_t dir )
{
    mtarget_steer = dir;

}

void IDriveController::set_target_steer_angle(int16_t turn_angle)
{
    mgeo_turning_angle = turn_angle;
}

bool IDriveController::get_forward(void)
{
    return mmoving_forward;
}

void IDriveController::set_forward(bool fwd)
{
    mmoving_forward=fwd;
}

float IDriveController::getTargetPercent(float afPercent, float afLowBound, float afUpperBound, bool bTowardUpper)
{
    LOGD("afPercent=%f afLowBound=%f\n", afPercent, afLowBound);
    if(afPercent < 0.0
       || afPercent > 45.0
       || (afLowBound > afUpperBound))
    {
        return 0.0;
    }
    float fVal = ((afPercent/45.0) * (afUpperBound - afLowBound));
    return bTowardUpper ? (afLowBound + fVal) : (afUpperBound - fVal);
}

void IDriveController::UpdateSpeedSteerSignal(void)
{
    LOGV("\n Updating Speed Steer CAN signal");
    SpeedSteerMsg.MASTER_SPEED_STEER_CMD_speed = (mmoving_forward) ? (int16_t)mtarget_speed : (-((int16_t)(mtarget_speed+mnReverseSpeedOffset))) ;

    if(mtarget_steer == steer_left)
        SpeedSteerMsg.MASTER_SPEED_STEER_CMD_steer = (mgeo_turning_angle < 45.0 )? (getTargetPercent(mgeo_turning_angle, -MAX_STEER_PERCENTAGE, 0, false)):(-((int16_t)MAX_STEER_PERCENTAGE));
    else if(mtarget_steer == steer_right)
        SpeedSteerMsg.MASTER_SPEED_STEER_CMD_steer = (mgeo_turning_angle < 45.0 )? (getTargetPercent(mgeo_turning_angle, 0, MAX_STEER_PERCENTAGE, true)) : (MAX_STEER_PERCENTAGE) ;
    else if(mtarget_steer == steer_straight)
        SpeedSteerMsg.MASTER_SPEED_STEER_CMD_steer = 0;

    mcurr_steer = mtarget_steer;
    mcurr_speed = mtarget_speed;
}

















/****************************************************************************
 * Revision History
 * @format : version/ Date / Author / Change description
 ****************************************************************************
 * Version  Date         Author    Change Log
 * 1        10-16-2017   Revathy   Created Initial file. Added interface file headers
 *                                 for motor,sensor and system
 ****************************************************************************/



