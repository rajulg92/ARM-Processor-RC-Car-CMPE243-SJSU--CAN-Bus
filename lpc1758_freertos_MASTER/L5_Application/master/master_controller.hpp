/****************************************************************************
 * @file  : master_controller.hpp
 * @brief : This file contains the implementation of Master Controller which
 *          is the brain of the autonomous R/C car. It integrates the functionality
 *          of all the system components (sensor,GPS,motor,android modules) and runs
 *          algorithm to maneuver the car through the selected route.
 * @created : Oct 16,2017
 ****************************************************************************/

#ifndef MASTER_CONTROLLER_HPP_
#define MASTER_CONTROLLER_HPP_

/* Header includes */
#include "../../_can_dbc/generated_can.h"
#include <master/drive_interface.hpp>
#include <master/geo_interface.hpp>
#include <master/sensor_io_interface.hpp>
#include <master/android_interface.hpp>
#include "can.h"

/* Macro Definitions*/
#define SYSTEM_NODES  (4)
#define SENSOR_LED    (0) //  In a 4 bit number, bit 1 indicates sensor node alive/dead
#define GEO_LED       (1) //  In a 4 bit number, bit 2 indicates geo node alive/dead
#define BLE_LED       (2) //  In a 4 bit number, bit 3 indicates android node alive/dead
#define MOTOR_LED     (3) //  In a 4 bit number, bit 4 indicates motor node alive/dead

#define TELE_SYS_STOP  (1)
#define TELE_SYS_START (2)
#define TELE_SYS_RUN   (3)

/* External Linkages*/

/* Function Declaration */

/* Class declaration */

/* Typedefs */

/*This enum indicates the system error code that maps the 4-bit error code set bits to on-board LEDs ON state*/
typedef enum{
    no_error = 0,
    CAN_bus_off = 3,
    sensor_heartbeat = 1, //LED steady state indicates heartbeat not received
    geo_heartbeat = 2,    //LED steady state indicates heartbeat not received
    ble_heartbeat = 4,
    motor_heartbeat=8,
    mia_sensor_data =1,  //LED toggle indicates mia occurred
    mia_geo_turnangle=2 //LED toggle indicates mia occurred
} led_error_code_t;

/*
 * @class: MasterController interfaces the master node to the system nodes in the network.
 *         All CAN communication with system nodes are handled by MasterController.
 *         The base-classes inherited by MasterController are dedicated classes to process
 *         the corresponding signals received from the system nodes.It fuses GPS signal and sensor signal
 *         and runs the obstacle avoidance algorithm to send drive control signals to motor node.
 * @methods : Implements methods to initialize and handle CAN Tx and Rx of all the messages, check for heartbeat signals
 *          from other controllers, run route maneuvering algorithm.
 */
class MasterController : public IDriveController,
                         public ISensorController,
                         public IGeoController,
                         public IAndroidController
{
    public:

    MasterController(void);
    bool InitializeCAN(void);
    void CheckCAN_Busoff(void);
    bool HandleCAN_RxMsg(uint32_t mia_incr_ms);
    bool HandleCAN_Mia(uint32_t incr_count);
    void HandleBleUserCommand(void);
    uint8_t SetLedErrorCode(led_error_code_t err_code);
    void SetLedDisplay(char leftdigit,char rightdigit);
    void SetObstacleAvoidanceOn(bool val);
    bool GetObstacleAvoidanceOn(void);
    bool isNetworkNodesAlive(void);
    void ResetHeartbeatIndicators(void);
    void ResetMasterTelemetryMsg(void);
    bool RunRouteManeuvering(void);
    steer_dir_t RunObstacleAvoidanceAlgo(obs_detection_t ob_status);
    bool Tx_MASTER_SPEED_STEER_CMD(void);
    bool Tx_MASTER_SYS_STOP_CMD(void);
    bool Tx_MASTER_TELEMETRY(void);

    MASTER_SYS_STOP_CMD_t MasterSysStopMsg;

    /*
     * All debug indications are recorded in this master telemetry message
     * MASTER_TELEMETRY_sys_status value gives the system running status
     * stop = 01, started = 02, running =03;
     */
    MASTER_TELEMETRY_t MasterTelemetryMsg;
    uint8_t mheartbeat_status;
    //MOTOR_HEARTBEAT_t MotorHeartbeatMsg;

    private:

    can_t mchannel;
    uint32_t mbaudrate;
    bool mobstacle_avoidance_on;
    bool mdestination_reached;
    uint8_t mcheckpoint_id;
    uint8_t mrxmsg_cnt;

};


#endif /* MASTER_CONTROLLER_HPP_ */

/****************************************************************************
 * Revision History
 * @format : version/ Date / Author / Change description
 ****************************************************************************
 * Version  Date         Author    Change Log
 * 1        10-16-2017   Revathy   Created Initial file. Added interface file headers
 *                                 for motor,sensor and system
 ****************************************************************************/
