/*
 * drive_manager.cpp
 *
 *  Created on: Oct 14, 2017
 *      Author: unnik
 */


#include "drive_manager.hpp"
#include "can.h"
#include <string.h>
#include "error_stat_display.hpp"
#include "scheduler_task.hpp"

//#define DEBUG_E
//#define VERBOSE_E
#include "debug_e.h"
#include "eint.h"
#include "gpio.hpp"

GPIO head_lights(P0_1);   // Control First Green_Connector From Left
GPIO right_lights(P1_22);
GPIO left_lights(P1_20);
GPIO break_lights(P1_19);

#define ABS(a) ((a > 0.0) ? (a) : (-(a)))
#define CAN_BUS (can2)

const uint32_t                             MASTER_SPEED_STEER_CMD__MIA_MS  = 10000;
/** default speed if Master does not drive for long:
 * (Master is expected to update us atleast every 10 seconds)
 * Speed: 0% (Neutral)
 * Steer: 0% (Straight) */
const MASTER_SPEED_STEER_CMD_t             MASTER_SPEED_STEER_CMD__MIA_MSG = {0, 0};

bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    can_msg_t msg = {0};
    msg.msg_id = mid;
    msg.frame_fields.data_len = dlc;
    msg.frame_fields.is_29bit = 0;
    msg.frame_fields.is_rtr = 0;
    memcpy((void*)&msg.data, bytes, sizeof(msg.data));
    return CAN_tx(CAN_BUS, &msg, 0);
}

void vCANBusOffCheckCallback(uint32_t anICR)
{
    /** Reset the bus */
    if(CAN_is_bus_off(CAN_BUS))
    {
        ErrorStatDisplay::displayErrorStat(kErrorStatDisplay_BusOff);
        LOGV("CAN bus off cb %x\n", anICR);
        CAN_reset_bus(CAN_BUS);
    }
    LOGD("CAN rx count = %d tx=%d rx_dropped=%d\n", CAN_get_rx_count(CAN_BUS), CAN_get_tx_count(CAN_BUS), CAN_get_rx_dropped_count(CAN_BUS));
}

void vCANBusDataOverrunCallback(uint32_t anICR)
{
    LOGD("CAN bus data overrun %x\n", anICR);
    ErrorStatDisplay::displayErrorStat(kErrorStatDisplay_BusOff);
}

void DriveManager::addControllerTask()
{
    DriveController* pDriveCtrlr = NULL;
    LOGV("pDriveCtrlr=%p\n", pDriveCtrlr);
    pDriveCtrlr = (DriveController*)(&DriveController::getInstance());
    scheduler_add_task(pDriveCtrlr);
}


bool DriveManager::initDriveManager()
{
    bool ret = false;
    /** initialize CAN */
    ret = CAN_init(CAN_BUS, 100, 10, 10, vCANBusOffCheckCallback, vCANBusDataOverrunCallback);
    if(ret)
    {
        LOGV("CAN init success\n");
        /** CAN Bus initialized; by default no msgs will be accepted */
        CAN_bypass_filter_accept_all_msgs();
        CAN_reset_bus(CAN_BUS);
    }
    else
    {
        LOGV("CAN init failed\n");
    }
    /** P0.1 shall be registered as GPIO where we shall
     * get an interrupt for each rotation */

    rpmSensor.enablePullDown();
    eint3_enable_port0(26, eint_rising_edge, vRPMSigReceive);

    // Car Lights Initialization
    car_lights_init();

    return ret;
}

bool DriveManager::handleCANIncoming(uint32_t anTimeIntervalMs)
{
    bool ret = true;
    can_msg_t msg;
    /** Receive from CAN */
    while(CAN_rx(CAN_BUS, &msg, 0))
    {
        LOGD("received CAN msg %d %x %x\n", msg.msg_id, msg.data.words[0], msg.data.qword);
        switch(msg.msg_id)
        {
            case MASTER_SPEED_STEER_CMD_HDR.mid:
                if(MASTER_SPEED_STEER_CMD_HDR.dlc == msg.frame_fields.data_len)
                {
                    dbc_decode_MASTER_SPEED_STEER_CMD(&speedSteerCmd, (msg.data.bytes), &MASTER_SPEED_STEER_CMD_HDR);
                    LOGD("Decoded SPEED_STEER_CMD msg %d %d \n", speedSteerCmd.MASTER_SPEED_STEER_CMD_speed,speedSteerCmd.MASTER_SPEED_STEER_CMD_steer);
                    ret |= receiveDriveCommands(&speedSteerCmd);
                }
            break;
            case MASTER_SYS_STOP_CMD_HDR.mid:
                /** Master is asking to stop the car */
                speedSteerCmd.MASTER_SPEED_STEER_CMD_speed = 0;
                speedSteerCmd.MASTER_SPEED_STEER_CMD_steer = 0;
                ret |= receiveDriveCommands(&speedSteerCmd);
            break;
            default:
                /** nothing to do here */
            break;
        }
    }
    bool mia_occured = dbc_handle_mia_MASTER_SPEED_STEER_CMD(&speedSteerCmd, anTimeIntervalMs);
    if(mia_occured)
    {
        ErrorStatDisplay::displayErrorStat(kErrorStatDisplay_SSCmdMIAOccured);
        LOGV("MIA occured; move drive back to Speed_Neutral + Steer_Straight\n");
        memset(&speedSteerCmd, 0, sizeof(MASTER_SPEED_STEER_CMD_t));
        ret |= receiveDriveCommands(&speedSteerCmd);
    }
    return ret;
}

void DriveManager::getDriveCmdInfo(MASTER_SPEED_STEER_CMD_t speedSteer,
        float& afSpeedPercent, float& afSteerPercent,
        eDriveSpeedCmd& aDriveSpeedCmd,
        eDriveSteerCmd& aDriveSteerCmd)
{
    aDriveSpeedCmd = kDriveSpeedCmd_Invalid;
    aDriveSteerCmd = kDriveSteerCmd_Invalid;
    afSpeedPercent = afSteerPercent = 0.0;
    /** speed */
    if(speedSteer.MASTER_SPEED_STEER_CMD_speed < 0
       && speedSteer.MASTER_SPEED_STEER_CMD_speed >= -100)
    {
        /** reverse */
        afSpeedPercent = (float)abs(speedSteer.MASTER_SPEED_STEER_CMD_speed);
        aDriveSpeedCmd = kDriveSpeedCmd_Rev;
    }
    if(speedSteer.MASTER_SPEED_STEER_CMD_speed == 0)
    {
        afSpeedPercent = 0.0;
        aDriveSpeedCmd = kDriveSpeedCmd_Neutral;
    }
    if(speedSteer.MASTER_SPEED_STEER_CMD_speed > 0
            && speedSteer.MASTER_SPEED_STEER_CMD_speed <= 100)
    {
        afSpeedPercent = (float)(speedSteer.MASTER_SPEED_STEER_CMD_speed);
        aDriveSpeedCmd = kDriveSpeedCmd_Fwd;
    }

    if(speedSteer.MASTER_SPEED_STEER_CMD_steer < 0
       && speedSteer.MASTER_SPEED_STEER_CMD_steer >= -100)
    {
        /** reverse */
        afSteerPercent = (float)abs(speedSteer.MASTER_SPEED_STEER_CMD_steer);
        aDriveSteerCmd = kDriveSteerCmd_Left;
    }
    if(speedSteer.MASTER_SPEED_STEER_CMD_steer == 0)
    {
        afSteerPercent = 0.0;
        aDriveSteerCmd = kDriveSteerCmd_Straight;
    }
    if(speedSteer.MASTER_SPEED_STEER_CMD_steer > 0
       && speedSteer.MASTER_SPEED_STEER_CMD_steer <= 100)
    {
        afSteerPercent = (float)(speedSteer.MASTER_SPEED_STEER_CMD_steer);
        aDriveSteerCmd = kDriveSteerCmd_Right;
    }
    return;
}
void car_lights_init()
{
       head_lights.setAsOutput(); // Use the pin as output pin
       right_lights.setAsOutput(); // Use the pin as output pin
       left_lights.setAsOutput();  // Use the pin as output pin
       break_lights.setAsOutput(); // Use the pin as output pin

       head_lights.setHigh();     // Pin will now be at 0.0v
       break_lights.setHigh();
       left_lights.setHigh();
       right_lights.setHigh();
       uint8_t lightsensor = LS.getPercentValue();

       // If Light is 15%, considering 100% as brightest day, Turn ON Headlights
       if (lightsensor < 15){
            head_lights.setLow();     // Pin will now be at 3.3v
       }
}
void break_lights_fun()
{
    break_lights.setLow();
    left_lights.setHigh();
    right_lights.setHigh();
}

void left_lights_fun()
{
    break_lights.setHigh();
    left_lights.toggle();
    right_lights.setHigh();
}

void right_lights_fun()
{
    break_lights.setHigh();
    left_lights.setHigh();
    right_lights.toggle();
}

void rev_lights_fun()
{
    break_lights.setHigh();
    left_lights.toggle();
    right_lights.toggle();
}
bool DriveManager::receiveDriveCommands(MASTER_SPEED_STEER_CMD_t* apSpeedSteer)
{
    bool ret = false;
    eDriveSpeedCmd driveSpeedCmd;
    eDriveSteerCmd driveSteerCmd;
    tDriveState driveStateNow = {0};
    float fLSpeedPercent;

    if(!apSpeedSteer)
        return ret;

    LOGD("speed=%d steer=%d\n", apSpeedSteer->MASTER_SPEED_STEER_CMD_speed, apSpeedSteer->MASTER_SPEED_STEER_CMD_steer);

    /** parse the incoming message and understand:
     * speed: rev/fwd/neutral
     * steer: left/right/straight
     * */
    getDriveCmdInfo(*apSpeedSteer, driveStateNow.fSpeedPercent, driveStateNow.fSteerPercent,
            driveSpeedCmd, driveSteerCmd);

    LOGD("driveSpeedCmd=%d driveSteerCmd=%d fSpeedPercent=%f fSteerPercent=%f\n",
            driveSpeedCmd, driveSteerCmd, driveStateNow.fSpeedPercent, driveStateNow.fSteerPercent);
    ErrorStatDisplay::displayDebugInt((char)(((char)(driveStateNow.fSpeedPercent))%10) + ((driveSpeedCmd == kDriveSpeedCmd_Rev) ? 10 : 0));

    driveStateNow.cmd = driveSpeedCmd | driveSteerCmd;
    if(memcmp(&driveState, &driveStateNow, sizeof(tDriveState)) == 0)
    {
        LOGD("received same state; ignoring\n");
        ret = true;
        return ret;
    }
    /** Only if the new command from master demands a change in ESC, we need to 
     * proceed with ESC controller */
    if(ABS(driveState.fSpeedPercent - driveStateNow.fSpeedPercent) > 0.0
       || ((driveState.cmd & kDriveSpeedCmd_Mask) != (driveStateNow.cmd & kDriveSpeedCmd_Mask)))
    {
        switch(driveSpeedCmd)
        {
            case kDriveSpeedCmd_Fwd:
                LOGD("speed fwd %f%%\n", driveStateNow.fSpeedPercent);
                updateTargetSpeedPercent(driveStateNow.fSpeedPercent);
    #if 1
                fLSpeedPercent = driveStateNow.fSpeedPercent;
                if(!(driveSteerCmd & kDriveSteerCmd_Straight))
                    fLSpeedPercent += ESC_SPEED_STEP;
    #endif
                driveCtrlr.escForward(fLSpeedPercent);
            break;
            case kDriveSpeedCmd_Rev:
                updateTargetSpeedPercent(driveStateNow.fSpeedPercent);
                LOGD("speed reverse %f%%\n", driveStateNow.fSpeedPercent);
                driveCtrlr.escReverseTraxxasXL5(driveStateNow.fSpeedPercent);
                rev_lights_fun();
            break;
            case kDriveSpeedCmd_Neutral:
                LOGD("speed neutral 0%%\n");
                driveCtrlr.escNeutral();
                break_lights_fun();
            break;
            default:
            break;
        }
    }
    switch(driveSteerCmd)
    {
        case kDriveSteerCmd_Right:
            ErrorStatDisplay::displayErrorStat(kErrorStatDisplay_ServoRight);
            LOGD("steer right %f%%\n", driveStateNow.fSteerPercent);
            driveCtrlr.servoRight(driveStateNow.fSteerPercent);
            right_lights_fun();
        break;
        case kDriveSteerCmd_Left:
            ErrorStatDisplay::displayErrorStat(kErrorStatDisplay_ServoLeft);
            LOGD("steer left %f%%", driveStateNow.fSteerPercent);
            driveCtrlr.servoLeft(driveStateNow.fSteerPercent);
            left_lights_fun();
        break;
        case kDriveSteerCmd_Straight:
            ErrorStatDisplay::clearErrorStat(kErrorStatDisplay_ServoRight);
            ErrorStatDisplay::clearErrorStat(kErrorStatDisplay_ServoLeft);
            LOGD("steer straight\n");
            driveCtrlr.servoStraight();
        break;
        default:
        break;
    }

    driveState = driveStateNow;
    ret = true;
    return ret;
}

bool DriveManager::sendHeartBeat()
{
    driveCtrlrHeartbeat.MOTOR_HEARTBEAT_signal = 0x00;
    return dbc_encode_and_send_MOTOR_HEARTBEAT(&driveCtrlrHeartbeat);
}

/** Diameter of the car wheel is 9.5cm */
static uint32_t gnRotations = 0;

uint32_t DriveManager::getRPS()
{
    return fRPS;
}

uint32_t DriveManager::getRPM()
{
    return nRPM;
}

float DriveManager::getSpeed()
{
    return fSpeed;
}

void DriveManager::computeTelemetry(uint32_t anRotations, uint32_t anTimeElapsedMs)
{
    if(!anTimeElapsedMs)
        fRPS = 0;
    else
        fRPS = ((float)anRotations * 1000.0) / anTimeElapsedMs;
    nRPM = fRPS * 60;
    /** speed in meter per second:
     * = RPM = 2 * PI * r = PI * D
     * in miles per hour
     * 1mile = 1.609 km
     *
     *
     * RPS * DIAM_CM * 22.0 / (7.0 * 1000)
     */
    fSpeed = (((float)fRPS * 22 * WHEEL_DIAMETER_CM * 60 * 60) / (100000 * 1.609)) / 7.0;
    MOTOR_TELEMETRY_t telemetry;
    telemetry.MOTOR_TELEMETRY_rps = fRPS;
    telemetry.MOTOR_TELEMETRY_speed_mph = fSpeed;
    telemetry.MOTOR_TELEMETRY_speed_percent = (uint8_t)fDerivedSpeedPercent;
    telemetry.MOTOR_TELEMETRY_target_rps = mapRPS[(uint32_t)getTargetSpeedPercent()];
    dbc_encode_and_send_MOTOR_TELEMETRY(&telemetry);
    return;
}

bool DriveManager::handleFeedback(uint32_t anCount, uint32_t anIntervalMs)
{
    uint32_t nTimeElapsedMs = (anCount - nFeedbackInitialCount) * anIntervalMs;

    computeTelemetry(gnRotations, nTimeElapsedMs);
#if 1
    if(gnRotations)
#endif
    {
        nFeedbackInitialCount = anCount;
        gnRotations = 0;
    }
#if 0
    else if(nTimeElapsedMs > 1000)
    {
        fRPS = 0;
        fSpeed = 0;
    }
#endif

    LOGD("%f %f\n", fRPS, mapRPS[(uint32_t)getTargetSpeedPercent()]);
    if(fRPS < mapRPS[(uint32_t)getTargetSpeedPercent()])
    {
        updateDerivedSpeedPercent(fDerivedSpeedPercent + ESC_SPEED_STEP);
    }
    else if(fRPS > mapRPS[(uint32_t)getTargetSpeedPercent()])
    {
        updateDerivedSpeedPercent(fDerivedSpeedPercent - ESC_SPEED_STEP);
    }

    return true;
}

void DriveManager::mapTraxxasSpeedPercentToRPS(uint32_t anSpeedIdx)
{
    if(anSpeedIdx < 13)
    {
        mapRPS[anSpeedIdx] = RPS_ZERO;
    }
    else if(anSpeedIdx < 20)
    {
        mapRPS[anSpeedIdx] = RPS_NORMAL;
    }
    else if(anSpeedIdx < 30)
    {
        mapRPS[anSpeedIdx] = RPS_FAST;
    }
    else if(anSpeedIdx < SPEED_LEVEL_COUNTS)
    {
        mapRPS[anSpeedIdx] = RPS_TURBO;
    }
    else
    {
        mapRPS[anSpeedIdx] = RPS_ZERO;
    }
}

uint8_t DriveManager::getRPSForSpeedPercent(uint32_t anSpeedIdx)
{
    if(anSpeedIdx >= SPEED_LEVEL_COUNTS)
        return 0;
    return mapRPS[anSpeedIdx];
}

void DriveManager::updateTargetSpeedPercent(float afTargetSpeedPercent)
{
    if(afTargetSpeedPercent >= MIN_SPEED_PERCENT && afTargetSpeedPercent <= MAX_SPEED_PERCENT)
    {
        fTargetSpeedPercent = afTargetSpeedPercent;
        /** fDerivedSpeedPercent shall be changed by the
         * wheel-feedback handler
         **/
        fDerivedSpeedPercent = fTargetSpeedPercent;
    }
}

float DriveManager::getTargetSpeedPercent()
{
    return fTargetSpeedPercent;
}

float DriveManager::getDerivedSpeedPercent()
{
    return fTargetSpeedPercent;
}

bool DriveManager::updateDerivedSpeedPercent(float afDerivedSpeedPercent)
{
    if(afDerivedSpeedPercent >= MIN_SPEED_PERCENT && afDerivedSpeedPercent <= MAX_SPEED_PERCENT)
    {
        switch(driveState.cmd & kDriveSpeedCmd_Mask)
        {
            case kDriveSpeedCmd_Fwd:
                if(afDerivedSpeedPercent < ESC_FORWARD_MIN
                   || afDerivedSpeedPercent > ESC_FORWARD_MAX)
                {
                    return false;
                }
                fDerivedSpeedPercent = afDerivedSpeedPercent;
                /** even if the derived speed is lesser than before,
                 * When the wheel steer is not straight add a STEP percent
                 * to maintain good turns;
                 * But, fDerivedSpeedPercent shall not be altered in this function
                 * with any STEPs
                 * Changing local variable afDerivedSpeedPercent
                 * is viable */
                if(!(driveState.cmd & kDriveSteerCmd_Straight))
                {
                    afDerivedSpeedPercent += ESC_SPEED_STEP_EXTRA_FOR_STEER;
                }

                driveCtrlr.escForward(afDerivedSpeedPercent);
                break;
            case kDriveSpeedCmd_Rev:
                if(afDerivedSpeedPercent < ESC_REVERSE_MIN
                   || afDerivedSpeedPercent > ESC_REVERSE_MAX)
                {
                    return false;
                }
                return false;
                fDerivedSpeedPercent = afDerivedSpeedPercent;
                driveCtrlr.escReverse(afDerivedSpeedPercent);
                break;
            default:
                return false;
        }
        return true;
    }
    return false;
}

void vRPMSigReceive(void)
{
    //ErrorStatDisplay::displayErrorStat(kErrorStatDisplay_SSCmdMIAOccured);
    gnRotations++;
}
