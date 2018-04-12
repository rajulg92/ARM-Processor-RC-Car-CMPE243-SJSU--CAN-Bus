/****************************************************************************
 * geo_interface.cpp
 * @file :
 * @created : Oct 16,2017
 *
 ****************************************************************************/

#include "../../_can_dbc/generated_can.h"
#include <master/geo_interface.hpp>

IGeoController::IGeoController():mturning_angle(0),mdistance_to_destination(0)
{
    GeoManeuveringMsg={0};
    GeoTeleCompassMsg={0};
}

int16_t IGeoController::GetTurningAngle()
{
    return mturning_angle;
}


void IGeoController::SetTurningAngle(uint16_t turnangle)
{
    mturning_angle = turnangle;
}












/****************************************************************************
 * Revision History
 * @format : version/ Date / Author / Change description
 ****************************************************************************
 * Version  Date         Author    Change Log
 * 1        10-16-2017   Revathy   Created Initial file
 *
 ****************************************************************************/
