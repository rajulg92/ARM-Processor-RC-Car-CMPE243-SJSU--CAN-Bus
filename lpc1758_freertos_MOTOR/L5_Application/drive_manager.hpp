/*
 * drive_manager.hpp
 * @brief The Optimus glue code implementation for
 * Drive Controller
 *  Created on: Oct 14, 2017
 *      Author: unnik
 */

#ifndef DRIVE_MANAGER_HPP_
#define DRIVE_MANAGER_HPP_

#include "drive_controller.hpp"
#include "_can_dbc/generated_can.h"
#include "gpio.hpp"

#define WHEEL_DIAMETER_CM (9.5)
#define SPEED_LEVEL_COUNTS (101)

#define RPS_ZERO (0)
#define RPS_NORMAL (0.25)
#define RPS_FAST (1.0)
#define RPS_TURBO (3.0)

#define MIN_SPEED_PERCENT (0.0)
#define MAX_SPEED_PERCENT (100.0)

#define ESC_FORWARD_MIN (13)
#define ESC_FORWARD_MAX (50)
#define ESC_REVERSE_MAX (100)
#define ESC_REVERSE_MIN (21)

#define ESC_SPEED_STEP (0.5)
#define ESC_SPEED_STEP_EXTRA_FOR_STEER (0.5)

void car_lights_init();
void break_lights_fun();
void left_lights_fun();
void right_lights_fun();
void rev_lights_fun();

typedef enum
{
    kDriveSpeedCmd_Invalid    = 0x00,
    kDriveSpeedCmd_Neutral    = 0x01,
    kDriveSpeedCmd_Fwd        = 0x02,
    kDriveSpeedCmd_Rev        = 0x04,
    kDriveSpeedCmd_Mask       = 0x0f
}eDriveSpeedCmd;

typedef enum
{
    kDriveSteerCmd_Invalid    = 0x00,
    kDriveSteerCmd_Straight   = 0x10,
    kDriveSteerCmd_Left       = 0x20,
    kDriveSteerCmd_Right      = 0x40,
    kDriveSteerCmd_Mask       = 0xf0
}eDriveSteerCmd;

typedef struct
{
    uint32_t cmd;
    float fSteerPercent;
    float fSpeedPercent;
}tDriveState;

class DriveManager
{
public:
    DriveController& driveCtrlr;
    DriveManager() : driveCtrlr(DriveController::getInstance()),
            speedSteerCmd({0}),
            driveState({0}),
            driveCtrlrHeartbeat({0}),
            rpmSensor(P0_26),
            nFeedbackInitialCount(0),
            fSpeed(0),
            fRPS(0.0),
            nRPM(0),
            fTargetSpeedPercent(0),
            fDerivedSpeedPercent(0.0),
            mapRPS{}
    {
        for(int i = 0; i < SPEED_LEVEL_COUNTS; i++)
        {
            mapTraxxasSpeedPercentToRPS(i);
        }
    }

    static void addControllerTask();

    /** getDriveCmdInfo */
    void getDriveCmdInfo(MASTER_SPEED_STEER_CMD_t speedSteer,
            float& afSpeedPercent, float& afSteerPercent,
            eDriveSpeedCmd& aDriveSpeedCmd,
            eDriveSteerCmd& aDriveSteerCmd);

    /** maintainDrive : PID for speed and steer */

    /**
     * @brief Checks for Optimus Master Controller commands
     * read CAN for drive/steer commands
     * with ESC commands:
     * a) Control Drive forward/reverse
     * b) with proper speed
     * with Servo commands:
     * a) Control Servo left, straight or neutral
     * b) By the provided angle
     */
    bool receiveDriveCommands(MASTER_SPEED_STEER_CMD_t* apSpeedSteer);
    bool sendHeartBeat();
    bool handleCANIncoming(uint32_t anTimeIntervalMs);
    bool initDriveManager();
    bool handleFeedback(uint32_t anCount, uint32_t anIntervalMs);
    void computeTelemetry(uint32_t anRotations, uint32_t anTimeElapsedMs);
    uint32_t getRPS();
    uint32_t getRPM();
    float getSpeed();
    /** map the speed percent to corresponding RPM
     * These values are found experimentally */
    void mapTraxxasSpeedPercentToRPS(uint32_t anSpeedIdx);
    uint8_t getRPSForSpeedPercent(uint32_t anSpeedIdx);
    /** Update the target speed percent given by Master controller
     * and compute the derived speed percent */
    void updateTargetSpeedPercent(float afTargetSpeedPercent);
    float getTargetSpeedPercent();
    float getDerivedSpeedPercent();
    bool updateDerivedSpeedPercent(float afDerivedSpeedPercent);
    tDriveState getDriveState();
    MASTER_SPEED_STEER_CMD_t speedSteerCmd;
    tDriveState driveState;
private:
    MOTOR_HEARTBEAT_t driveCtrlrHeartbeat;
    GPIO rpmSensor;
    uint32_t nFeedbackInitialCount;
    double fSpeed;
    float fRPS;
    uint32_t nRPM;
    /** The target speed percent given by Master controller */
    float fTargetSpeedPercent;
    /** The speed percent derived from telemetry feedback */
    float fDerivedSpeedPercent;
    float mapRPS[SPEED_LEVEL_COUNTS];
};

#ifdef __cplusplus
extern "C" {
#endif

void vCANBusOffCheckCallback(uint32_t anICR);

void vCANBusDataOverrunCallback(uint32_t anICR);

void vRPMSigReceive(void);

#ifdef __cplusplus
}
#endif


#endif /* DRIVE_MANAGER_HPP_ */
