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
#include "io.hpp"
#include "periodic_callback.h"
#include "gpio.hpp"
#include "drive_manager.hpp"

//#define VERBOSE_E
#include "debug_e.h"

static DriveManager* gpDriveManager = NULL;

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
    gpDriveManager = new DriveManager();
    gpDriveManager->initDriveManager();
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
#ifdef LED_DBG
    LE.toggle(1);
#endif
    vCANBusOffCheckCallback(0);
    gpDriveManager->sendHeartBeat();

    LOGV("RPS=%d\n", gpDriveManager->getRPS());
}

void period_10Hz(uint32_t count)
{
#ifdef LED_DBG
    LE.toggle(2);
#endif
    gpDriveManager->handleFeedback(count, 100);
}

void period_100Hz(uint32_t count)
{
#ifdef LED_DBG
    LE.toggle(3);
#endif
    gpDriveManager->handleCANIncoming(10);
}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
#ifdef LED_DBG
    LE.toggle(4);
#endif
}

/** Enable this macro for 
 * drive Terminal command handler
 */
#define TEST_MOTOR_STANDALONE

#ifdef TEST_MOTOR_STANDALONE
#include "handlers.hpp"
CMD_HANDLER_FUNC(driveHandler)
{
   int value=0;
   unsigned int option=0;
   cmdParams.scanf("%u %d", &option, &value);

   switch(option)
   {
      case 1:
             LOGV("Stop\n");
             gpDriveManager->updateTargetSpeedPercent(0);
             gpDriveManager->driveState.cmd = kDriveSpeedCmd_Neutral | kDriveSteerCmd_Straight;
             gpDriveManager->driveCtrlr.escNeutral();
             gpDriveManager->driveCtrlr.servoStraight();
             break;
      case 2:LOGV("Fwd\n");
             gpDriveManager->driveState.cmd = kDriveSpeedCmd_Fwd | kDriveSteerCmd_Straight;
             gpDriveManager->updateTargetSpeedPercent(value);
             gpDriveManager->driveCtrlr.escForward(value);
             break;
      case 3:LOGV("Rev %d\n", value);
             gpDriveManager->driveState.cmd = kDriveSpeedCmd_Rev | kDriveSteerCmd_Straight;
             gpDriveManager->updateTargetSpeedPercent(value);
             gpDriveManager->driveCtrlr.escReverseTraxxasXL5(value);
             break;
      case 4:LOGV("Left\n");
             gpDriveManager->driveCtrlr.servoLeft(value);
             break;
      case 5:LOGV("Right\n");
             gpDriveManager->driveCtrlr.servoRight(value);
             break;
      default:LOGV("Invalid entry\n");
             break;
    }
    return true;
}
#else
CMD_HANDLER_FUNC(driveHandler)
{
    return false;
}
#endif /**< TEST_MOTOR_STANDALONE */

