/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * This contains the period callback functions for the periodic scheduler
 *
 * @warning
 * These callbacks should be used for hard real-time system, and the priority of these
 * tasks are above everything else in the system (above the PRIORITY_CRITICAL).
 * The period functions SHOULD NEVER block and SHOULD NEVER run over their time slot.
 * For example, the 1000Hz take slot runs periodically every 1ms, and whatever you
 * do must be completed within 1ms.  Running over the time slot will reset the system.
 */

#include <stdint.h>
#include <stdio.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "debug.h"
#include "master/master_controller.hpp"
#include "handlers.hpp"


static MasterController* pmaster= NULL;
static bool enable_test_mode=false;
bool destination_check_enable = true;
bool checkpoint_check_enable = false;

/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

/**
 * This is the stack size of the dispatcher task that triggers the period tasks to run.
 * Minimum 1500 bytes are needed in order to write a debug file if the period tasks overrun.
 * This stack size is also used while calling the period_init() and period_reg_tlm(), and if you use
 * printf inside these functions, you need about 1500 bytes minimum
 */
const uint32_t PERIOD_MONITOR_TASK_STACK_SIZE_BYTES = (512 * 3);

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    pmaster = new MasterController;
    pmaster->InitializeCAN();

    return true; // Must return true upon success
}

/// Register any telemetry variables
bool period_reg_tlm(void)
{
    // Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
    return true; // Must return true upon success
}


/**
 * Below are your periodic functions.
 * The argument 'count' is the number of times each periodic task is called.
 */

void period_1Hz(uint32_t count)
{
    /* Every 1 second,
     * 1. the system bus and node status is checked
     * 2. Reset the Indicators for next cycle status update
     * 3. Handle system fault when a node is dead
     */
    pmaster->CheckCAN_Busoff(); //check whether callback can handle this

    //check for all controller's heart beat
    if(false == pmaster->isNetworkNodesAlive())
    {
        //Handle dead_node
       // pmaster->Tx_MASTER_SYS_STOP_CMD();

    }

    /*Every 1s Telemetry message is sent to ble controller*/
    pmaster->Tx_MASTER_TELEMETRY();

    //Reset Indicators
    pmaster->ResetHeartbeatIndicators();
    pmaster->ResetMasterTelemetryMsg();

    if(true == SW.getSwitch(4)) //ToDo change to external switch event
    {
        pmaster->set_sys_start(true);
    }

    if(true == SW.getSwitch(3)) //ToDo change to external switch event
    {
       bool enable = pmaster->GetObstacleAvoidanceOn();
       enable = !(enable);
       pmaster->SetObstacleAvoidanceOn(enable);
    }

    if(true == SW.getSwitch(2)) //ToDo change to external switch event
    {
        destination_check_enable = !(destination_check_enable);
    }

    if(true == SW.getSwitch(1))
    {
        checkpoint_check_enable = !(checkpoint_check_enable);
    }
}

void period_10Hz(uint32_t count)
{

}

void terminal_run(void)
{
    pmaster->mheartbeat_status = 0x0f;
}

void period_100Hz(uint32_t count)
{
    /*All master controller tasks run in 100Hz task */
    /* Enable test mode flag is used for testing through Terminal*/

    pmaster->HandleCAN_RxMsg(10);

    /*Check for system start flag before running maneuvering*/
    if(true == pmaster->get_sys_start())
    {
        pmaster->MasterTelemetryMsg.MASTER_TELEMETRY_sys_status = TELE_SYS_RUN;
        pmaster->RunRouteManeuvering();
    }
    else
    {
        pmaster->MasterTelemetryMsg.MASTER_TELEMETRY_sys_status = TELE_SYS_STOP;
    }
}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
    LE.toggle(4);
}

CMD_HANDLER_FUNC(mockSensorController)
{
    uint32_t track[4]={0};
    cmdParams.scanf("%*s %d %d %d %d",&track[0],&track[1],&track[2],&track[3]);

    if(enable_test_mode)
    {
    pmaster->LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR0 = track[0];
    pmaster->LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR3 = track[1];
    pmaster->LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR6 = track[2];
    pmaster->LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR9 = track[3];
    }

    return true;
}

CMD_HANDLER_FUNC(mockGeoController)
{
    int32_t turn_angle =0;
    cmdParams.scanf("%d", &turn_angle);
    //if(enable_test_mode)
    {
    pmaster->SetTurningAngle((int16_t)turn_angle);
    }
    return true;
}

CMD_HANDLER_FUNC(mockBLEController)
{
    uint32_t startcmd =0;
    cmdParams.scanf("%*s %u", &startcmd);
    if(enable_test_mode)
    {
    pmaster->BleStartStopMsg.BLE_START_STOP_CMD_start = startcmd;
    pmaster->HandleBleUserCommand();
    }
    return true;
}

CMD_HANDLER_FUNC(enableRCTestMode)
{
    uint32_t status=0;
    cmdParams.scanf("%*s %u", &status);

    if(status > 0)
    {
        enable_test_mode = true;
    }
    else
    {
        enable_test_mode = false;
    }

    return true;
}
