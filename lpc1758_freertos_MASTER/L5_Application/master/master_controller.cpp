/****************************************************************************
 * master_controller.cpp
 * @file : This is the common header file for all the modules supported by
 *         Master controller.
 * @created : Oct 16,2017
 *
 ****************************************************************************/


#include <stdint.h>
#include <stdio.h>
#include <master/master_controller.hpp>
#include "io.hpp"
#include <lpc_pwm.hpp>
#include <gpio.hpp>

//#define VERBOSE_E
//#define DEBUG_E
#include "debug.h"

/* Macro definition*/
#define TIMEOUT_0MS   (0)
#define MAX_TURN      (45)

const uint32_t                             SENSOR_LIDAR_OBSTACLE_INFO__MIA_MS = 1000;
const SENSOR_LIDAR_OBSTACLE_INFO_t         SENSOR_LIDAR_OBSTACLE_INFO__MIA_MSG = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
const uint32_t                             GEO_TURNING_ANGLE__MIA_MS = 1000;
const GEO_TURNING_ANGLE_t                  GEO_TURNING_ANGLE__MIA_MSG = {0};
const uint32_t                             GEO_TELECOMPASS__MIA_MS = 1000;
const GEO_TELECOMPASS_t                    GEO_TELECOMPASS__MIA_MSG = {0,0,0,1,0};

/* Global variables */
GPIO obstacle_indicator_pin(P2_0);
extern bool destination_check_enable;
//extern bool checkpoint_check_enable;

extern "C"
{ /* extern C*/

void bus_off_clbck(uint32_t);
void data_overrun_clbck(uint32_t);

void bus_off_clbck(uint32_t)
{
    LOGD("\n CAN bus off callback");
}


void data_overrun_clbck(uint32_t)
{
    LOGD("\n Data over run callback");
}

} /* extern C*/

MasterController::MasterController():mheartbeat_status(0),mchannel(can2),mbaudrate(100),mobstacle_avoidance_on(true),
                                     mdestination_reached(false),mcheckpoint_id(0),mrxmsg_cnt(0)
{
    LOGV("MasterController Object Instantiated");
    MasterSysStopMsg = {0};
    MasterTelemetryMsg = {0};
}

bool MasterController::InitializeCAN()
{
    LOGV("InitializeCAN");
    uint16_t rxq_size = 15;
    uint16_t txq_size = 10 ;
    can_void_func_t bus_off_cb= &bus_off_clbck;
    can_void_func_t data_ovr_cb = &data_overrun_clbck;

    CAN_init(mchannel,mbaudrate,rxq_size, txq_size,bus_off_cb,data_ovr_cb);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(mchannel);

    // GPIO Initialize
    obstacle_indicator_pin.setAsOutput();


    return true;
}

/*
 * @signature : void CAN_check_busoff_state(void)
 * @func : Monitors CAN bus state @1Hz and turns on CAN Bus if its in
 *         Bus off state
 * @param : None
 */
void MasterController::CheckCAN_Busoff(void)
{
    if(CAN_is_bus_off(mchannel) == true)
    {
        /*Indicate CAN bus is off through LED blink*/
        LE.toggle(3);

        /* Reset CAN Bus*/
        CAN_reset_bus(mchannel);
    }
    else
    {
        LE.set(3,false);
    }
}

void MasterController::ResetHeartbeatIndicators()
{
    LOGD("\n Heart beat Status code",mheartbeat_status);
    mheartbeat_status = 0;
    mrxmsg_cnt =0;
}

void MasterController::ResetMasterTelemetryMsg()
{
    MasterTelemetryMsg.MASTER_TELEMETRY_gps_mia = 0;
    MasterTelemetryMsg.MASTER_TELEMETRY_sensor_mia =0;
    MasterTelemetryMsg.MASTER_TELEMETRY_sys_status =0;
    MasterTelemetryMsg.MASTER_TELEMETRY_ble_heartbeat=0;
    MasterTelemetryMsg.MASTER_TELEMETRY_geo_heartbeat=0;
    MasterTelemetryMsg.MASTER_TELEMETRY_motor_heartbeat=0;
    MasterTelemetryMsg.MASTER_TELEMETRY_sensor_heartbeat=0;
    MasterTelemetryMsg.MASTER_TELEMETRY_gps_tele_mia = 0;
}

bool MasterController::HandleCAN_Mia(uint32_t incr_count)
{
    uint32_t count = incr_count;
    bool mia_occured = false;
    if(true == dbc_handle_mia_GEO_TURNING_ANGLE(&GeoManeuveringMsg,count))
    {
        mia_occured=true;

        MasterTelemetryMsg.MASTER_TELEMETRY_gps_mia = 1;
    }
    else
    {
    }

    if(true == dbc_handle_mia_SENSOR_LIDAR_OBSTACLE_INFO(&LidarObstacleMsg,count))
    {
        mia_occured=true;
        //LOGE("LIDAR MIA\n");
        MasterTelemetryMsg.MASTER_TELEMETRY_sensor_mia =1;
    }
    else
    {
    }

    if(true == dbc_handle_mia_GEO_TELECOMPASS(&GeoTeleCompassMsg,count))
    {
        mia_occured=true;
        MasterTelemetryMsg.MASTER_TELEMETRY_gps_tele_mia =1;
    }
    else
    {
    }

    return mia_occured;
}

void MasterController::HandleBleUserCommand(void)
{
   if(HandleUserStartStopCmd())
   {
       if(get_sys_start())
       {
           MasterTelemetryMsg.MASTER_TELEMETRY_sys_status = TELE_SYS_START;
           LOGD("\n handling user start command");
           set_target_speed(speed_slow,true);
           set_target_steer(steer_straight);
           UpdateSpeedSteerSignal();
           Tx_MASTER_SPEED_STEER_CMD();
       }
       else
       {
          MasterTelemetryMsg.MASTER_TELEMETRY_sys_status = TELE_SYS_STOP;
          LOGD("\n handling user stop command");
          Tx_MASTER_SYS_STOP_CMD();   //Motor is yet to support this command.
       }
   }
}

bool MasterController::HandleCAN_RxMsg(uint32_t mia_incr_ms)
{
    can_msg_t rx_msg = {0};
    dbc_msg_hdr_t hdr = {0};
    while(CAN_rx(mchannel,&rx_msg,TIMEOUT_0MS))
    {
        ++mrxmsg_cnt;
        hdr.dlc = rx_msg.frame_fields.data_len;
        hdr.mid = rx_msg.msg_id;
        switch(rx_msg.msg_id)
        {
            case MOTOR_HEARTBEAT_HDR.mid:
               mheartbeat_status |= (0x01 << MOTOR_LED);
               MasterTelemetryMsg.MASTER_TELEMETRY_motor_heartbeat=1;
               break;

            case SENSOR_HEARTBEAT_HDR.mid:
               mheartbeat_status |= (0x01 << SENSOR_LED);
               MasterTelemetryMsg.MASTER_TELEMETRY_sensor_heartbeat=1;
               break;

            case GEO_HEARTBEAT_HDR.mid:
               mheartbeat_status |= (0x01 << GEO_LED);
               MasterTelemetryMsg.MASTER_TELEMETRY_geo_heartbeat=1;
               break;

            case BLE_HEARTBEAT_HDR.mid:
               mheartbeat_status |= (0x01 << BLE_LED);
               MasterTelemetryMsg.MASTER_TELEMETRY_ble_heartbeat=1;
               break;

            case SENSOR_LIDAR_OBSTACLE_INFO_HDR.mid:
               dbc_decode_SENSOR_LIDAR_OBSTACLE_INFO(&LidarObstacleMsg,
                                                   rx_msg.data.bytes,
                                                   &hdr);
               break;

            case BLE_START_STOP_CMD_HDR.mid:
               dbc_decode_BLE_START_STOP_CMD(&BleStartStopMsg,
                                              rx_msg.data.bytes,
                                             &hdr);
               HandleBleUserCommand();
               break;

            case GEO_TURNING_ANGLE_HDR.mid:
               dbc_decode_GEO_TURNING_ANGLE(&GeoManeuveringMsg,
                                             rx_msg.data.bytes,
                                             &hdr);
               SetTurningAngle(GeoManeuveringMsg.GEO_TURNING_ANGLE_degree);
               break;

           case GEO_TELECOMPASS_HDR.mid:
               dbc_decode_GEO_TELECOMPASS(&GeoTeleCompassMsg,
                                          rx_msg.data.bytes,
                                          &hdr);
               mdestination_reached = (bool)GeoTeleCompassMsg.GEO_TELECOMPASS_destination_reached;
               mcheckpoint_id = GeoTeleCompassMsg.GEO_TELECOMPASS_checkpoint_id;
               if(mcheckpoint_id > 0 && mcheckpoint_id < 5)
               {
                   set_sys_start(true);
               }
               break;


            default:
                break;
        }
        LOGV("\n Total received CAN messages in a cycle : %d",mrxmsg_cnt);
        rx_msg.data.qword = 0;
        rx_msg.msg_id = 0;
        rx_msg.frame = 0;
        rx_msg.frame_fields.data_len = 0;
    }
    /*handle mia for sensor and geo messages*/
    return HandleCAN_Mia(mia_incr_ms);
}

void MasterController::SetObstacleAvoidanceOn(bool val)
{
    mobstacle_avoidance_on = val;
}

bool MasterController::GetObstacleAvoidanceOn()
{
   return mobstacle_avoidance_on;
}

/*
 * @brief : The 4 on-board LEDs are mapped to 4 bit error code. So 16 different error codes are indicated.
 *          LED on indicates error code bit is set, LED off indicates error code bit is unset
 *          eg. if the error code is 5, then LEDs 1 and 3 are ON, 2 and 4 are Off.
 *          This helps in interpreting the LED state as bit state.
 * @func : SetLedErrorCode method turns on/off LEDs corresponding to error code bits
 */

uint8_t MasterController::SetLedErrorCode(led_error_code_t err_code)
{
    uint8_t error_code = (uint8_t)(err_code & 0x0f);
    uint8_t bit_position = 1;
    uint8_t leds_on =0;

    //get bit position that are set to 1 to indicate in led
    for(int i=0; i<4;i++)
    {
        if( (error_code & 0x01)==0x01 )
        {
            leds_on++;
            LE.set(bit_position,true);
        }
        else
        {
            LE.set(bit_position,false);
        }
        bit_position +=1;
        error_code >>= 1;
    }
    return leds_on;
}

/*
 * @brief : mheartbeat_status member variable should be 0x00001111 if all the 4 controller's
 *         heart beat is received in 1s. If not the on board LED indicators should be toggling
 *         LED indicators are mapped in the Macro SENSOR_LED,GEO_LED,BLE_LED,MOTOR_LED.
 * @func : This method checks for unset bits in mheartbeat_status and turn on the corresponding
 *           LED indicator.
 */
bool MasterController::isNetworkNodesAlive()
{
    uint8_t bit_position = 1;
    /* 00001111 - indicates all 4 controllers heartbeat is received
     * if any of the controllers bit is not set, the corresponding LED is turned on.
     */
    if(0x0f != (mheartbeat_status & 0x0f))
    {
        //get bit position that are set to 1
        for(int i=0; i<4;i++)
        {

            if( (mheartbeat_status & 0x01) != 0x01 )
            {
                LE.set(bit_position,true);
            }
            else
            {
                LE.set(bit_position,false);
            }
            ++bit_position;
            mheartbeat_status >>= 1;
        }
        return false;
    }
    else
    {
       SetLedErrorCode(no_error);
    }

    return true;
}

MasterController::steer_dir_t MasterController::RunObstacleAvoidanceAlgo(obs_detection_t aobs_status)
{

    obs_detection_t detection = aobs_status;
    steer_dir_t atarget_steer = get_target_steer();
    bool moving_forward = get_forward();
    obstacle_indicator_pin.setLow();
    if(detection.no_obstacle == true)
    {
      //  obstacle_indicator_pin.setLow();
        LOGV("\n No obstacle detected");
        return get_target_steer();
    }
    else
    {

    }

    switch(atarget_steer)
    {
        case steer_right:
            if(moving_forward)
            {
                if(detection.frontright || detection.front)
                {
                    //ToDo turn on buzzer
                    obstacle_indicator_pin.setHigh();

                    if(!detection.front)
                    {
                        set_target_steer(steer_straight);
                        set_target_speed(speed_slow, true);
                    }
					else if(!detection.frontleft)
                    {
                       set_target_steer(steer_left);
                       set_target_steer_angle(MAX_TURN);
                    }
                    else if((!detection.rear))
                    {
                       set_target_steer(steer_straight);
                       set_target_speed(speed_slow, false);
                    }
                    else
                    {
                        //Tx_MASTER_SYS_STOP_CMD();
                        set_target_speed(stop, true);
                    }
                }
            }
            else
            {
                if(detection.rearright)
                {
                    //ToDo Turn on buzzer
                    obstacle_indicator_pin.setHigh();

                    if(!detection.rearleft)
                    {
                       set_target_steer(steer_left);
                       set_target_steer_angle(MAX_TURN);
                    }
                    else if(!detection.rear)
                    {
                        set_target_steer(steer_straight);

                    }
                    else if((!detection.front))
                    {
                       set_target_steer(steer_straight);
                       set_target_speed(speed_slow, true);
                    }
                    else
                    {
                        //Tx_MASTER_SYS_STOP_CMD();
                        set_target_speed(stop, true);
                    }
                }
            }
            break;

        case steer_left:
            if(moving_forward)
            {
                if(detection.frontleft || detection.front)
                {
                    //ToDo Turn on buzzer
                    obstacle_indicator_pin.setHigh();

                    if(!detection.front)
                    {
                       set_target_steer(steer_straight);
                    }
					else if(!detection.frontright)
                    {
                       set_target_steer(steer_right);
                       set_target_steer_angle(MAX_TURN);
                    }
					else if((!detection.rear))
                    {
                       set_target_steer(steer_straight);
                       set_target_speed(speed_slow, false);
                    }
                    else if(!detection.front)
                    {
                        set_target_steer(steer_straight);
                        set_target_speed(speed_slow, true);
                    }
                    else
                    {
                        //Tx_MASTER_SYS_STOP_CMD();
                        set_target_speed(stop, true);
                    }
                }
            }
            else
            {
                if(detection.rearleft)
                {
                    //ToDo Turn on buzzer
                    obstacle_indicator_pin.setHigh();

                    if(!detection.rearright)
                    {
                       set_target_steer(steer_right);
                       set_target_steer_angle(MAX_TURN);
                    }
                    else if((!detection.front))
                    {
                       set_target_steer(steer_straight);
                       set_target_speed(speed_slow, true);
                    }
                    else
                    {
                        //Tx_MASTER_SYS_STOP_CMD();
                        set_target_speed(stop, true);
                    }
                }
            }
            break;

        case steer_straight:
            if(moving_forward)
            {
                if(detection.front)
                {
                    //ToDo Turn on buzzer
                    obstacle_indicator_pin.setHigh();

                    if(!detection.frontright)
                    {
                        set_target_steer(steer_right);
                        set_target_steer_angle(MAX_TURN);
                    }
                    else if(!detection.frontleft)
                    {
                        set_target_steer(steer_left);
                        set_target_steer_angle(MAX_TURN);
                    }
                    else if(!detection.rear)
                    {
                        set_target_speed(speed_slow,false);
                    }
                    else
                    {
                        set_target_speed(stop,true);
                    }

                    //ToDo rear right/left

                }
                else
                {
                    //obstacle_indicator_pin.setHigh();
                    //Tx_MASTER_SYS_STOP_CMD();
                    //set_target_speed(stop, true);
                }
            }
            else
            {
               if(detection.rear)
               {
                   //ToDo Turn on buzzer
                  obstacle_indicator_pin.setHigh();

                  if(!detection.rearright)
                  {
                      set_target_steer(steer_right);
                      set_target_steer_angle(MAX_TURN);
                  }
                  else if(!detection.rearleft)
                  {
                      set_target_steer(steer_left);
                      set_target_steer_angle(MAX_TURN);
                  }
                  else if(!detection.front)
                  {
                      set_target_speed(speed_slow, true);
                  }
                  else
                  {
                      set_target_speed(stop,false);
                  }
               }
               else
               {
                  // obstacle_indicator_pin.setHigh();
                   //Tx_MASTER_SYS_STOP_CMD();
                   //set_target_speed(stop, true);
               }
            }
            break;

        default:
            break;

    }
    return get_target_steer();
}

bool MasterController::RunRouteManeuvering()
{
    bool ret = true;
    obs_detection_t obs_status= {0};
    int16_t geo_turning_angle = 0;
    LOGV("\n Starting Maneuvering");

    //ToDo : Check the exact sequence start from Android App to System_Start_cmd signal
    /*
     * Design: Home screen has buttons - Start/Stop , BT , Navigate , Info , Dash board, Debug view
     *
     * When app starts, only BT,Info button is enabled, all other is disabled
     * When BT paired, Enable DestinationSelection Nav,Dashboard,Debugview button.
     * When Nav selected, Enable start/stop button.
     */

    // Get GPS turning angle to set target steer direction
    geo_turning_angle = GetTurningAngle();

    //ignore {-10,+10} values from geo turning angle to avoid error
    if(geo_turning_angle > 10)
    {
       set_target_steer(steer_right);
       set_target_steer_angle(abs(geo_turning_angle));


    }
    else if(geo_turning_angle < -10)
    {
       set_target_steer(steer_left);
       set_target_steer_angle(abs(geo_turning_angle));
    }
    else
    {
       set_target_steer(steer_straight);
    }

    // Check Distance to destination to set speed
    set_target_speed(speed_slow,true);  // Need to add condition

    // Run Obstacle detection algorithm
    obs_status = RunObstacleDetectionAlgo();

    //correct/modify set speed and steer direction based on obstacle detection
    if(mobstacle_avoidance_on == true )
    {
        RunObstacleAvoidanceAlgo(obs_status);
    }

    if((mdestination_reached == true) && (destination_check_enable == true))
    {
        set_target_speed(stop,true);
        set_sys_start(false);

    }
    else
    {
    }

    // Update Motor speed and steer CAN signal with the new calculated values
    UpdateSpeedSteerSignal();

    // Tx the CAN message to drive controller
    ret = Tx_MASTER_SPEED_STEER_CMD();

    return ret;
}

void MasterController::SetLedDisplay(char aleftdigit,char arightdigit)
{
    char l = aleftdigit;
    char r = arightdigit;
    LD.setRightDigit(r);
    LD.setLeftDigit(l);
}


bool MasterController::Tx_MASTER_SPEED_STEER_CMD()
{
    can_msg_t msg = {0};

    //Displays Left digit as target speed, right digit as target steer in LED Display
    char rightdigit = '0'+ (uint8_t)(get_target_steer());
    char leftdigit = '0'+ (((uint8_t)get_target_speed()) / 10);

    if(!get_forward())
    {
        leftdigit = leftdigit + 4;

    }

    dbc_msg_hdr_t msg_hdr = dbc_encode_MASTER_SPEED_STEER_CMD(msg.data.bytes , &SpeedSteerMsg);
    SetLedDisplay(leftdigit,rightdigit);

    msg.msg_id = msg_hdr.mid;
    msg.frame_fields.is_29bit = 0;
    msg.frame_fields.data_len = msg_hdr.dlc;

    return CAN_tx(mchannel,&msg,0);
}

bool MasterController::Tx_MASTER_SYS_STOP_CMD()
{
    can_msg_t msg = {0};
    MasterSysStopMsg.MASTER_SYS_STOP_CMD_stop = 1;

    dbc_msg_hdr_t msg_hdr = dbc_encode_MASTER_SYS_STOP_CMD(msg.data.bytes , &MasterSysStopMsg);

    msg.msg_id = msg_hdr.mid;
    msg.frame_fields.is_29bit = 0;
    msg.frame_fields.data_len = msg_hdr.dlc;

    /* This sets the member "msys_start" in IAndroidController to indicate the system is stopped by Master
     * This over rides the user start/stop command in case of system fault detected.
     * */
    set_sys_start(false);

    return CAN_tx(mchannel,&msg,0);
}

bool MasterController::Tx_MASTER_TELEMETRY(void)
{
    can_msg_t msg = {0};

    dbc_msg_hdr_t msg_hdr = dbc_encode_MASTER_TELEMETRY(msg.data.bytes , &MasterTelemetryMsg);

    msg.msg_id = msg_hdr.mid;
    msg.frame_fields.is_29bit = 0;
    msg.frame_fields.data_len = msg_hdr.dlc;

    return CAN_tx(mchannel,&msg,0);

}

/****************************************************************************
 * Revision History
 * @format : version/ Date / Author / Change description
 ****************************************************************************
 * Version  Date         Author    Change Log
 * 1        10-16-2017   Revathy   Created Initial file. Added interface file headers
 *                                 for motor,sensor and system
 * 2        10-20-2017   Revathy   Added MasterController method definition
 * 3        10-24-2017   Revathy   Added MIA handling for motor heart beat
 * 4        10-27-2017   Revathy   Added Obstacle avoidance algorithm - version1
 ****************************************************************************/
