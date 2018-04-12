/****************************************************************************
 * system_interface.cpp

 * @file :
 * @created : Oct 16,2017
 *
 ****************************************************************************/
#include "../../_can_dbc/generated_can.h"
#include <master/android_interface.hpp>
#include <stdint.h>
#include <stdio.h>


/* Macro Definitions*/

/* Global & External Variables Definition*/

/* File scope Declaration */

/* Method definition*/
IAndroidController::IAndroidController():msys_start(false)
{
    BleStartStopMsg = {0};
}

bool IAndroidController::HandleUserStartStopCmd()
{
   if(BleStartStopMsg.BLE_START_STOP_CMD_start == msys_start )
   {
      return false;
   }
   else
   {
       msys_start =BleStartStopMsg.BLE_START_STOP_CMD_start ;
       return true;
   }
}

bool IAndroidController::get_sys_start()
{
    return msys_start;
}

void IAndroidController::set_sys_start(bool val)
{
     msys_start=val;
}






















/****************************************************************************
 * Revision History
 * @format : version/ Date / Author / Change description
 ****************************************************************************
 * Version  Date         Author    Change Log
 * 1        10-16-2017   Revathy   Created Initial file
 * 2        10-27-2017   Revathy   Defined HandleUserStartStopCmd()
 ****************************************************************************/
