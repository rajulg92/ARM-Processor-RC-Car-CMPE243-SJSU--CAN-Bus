/*
 * SensorControllerCAN_Tx.cpp
 *
 *  Created on: Oct 28, 2017
 *      Author: Harshitha, Sushma, Supradeep
 */

#include "SensorControllerCAN_Tx.hpp"
#include "can.h"
#include "_can_dbc/generated_can.h"
#include "error_stat_display.hpp"

//Initializes the CAN bus
void SensorControllerCAN_Tx::InitLidarCAN()
{
    CAN_init(can_bus_number, 100, 16, 16, NULL, NULL);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(can_bus_number);
}

//Resets the CAN bus if it is off
void SensorControllerCAN_Tx::CanTxLidarBusOffAction()
{
    if (CAN_is_bus_off(can_bus_number))
    {
        ErrorStatDisplay::displayErrorStat(kErrorStatDisplay_BusOff);
        CAN_reset_bus(can_bus_number);
    }
    else{
        ErrorStatDisplay::clearErrorStat(kErrorStatDisplay_BusOff);
    }
}

//Transmits the Heart beat
void SensorControllerCAN_Tx::CanTxLidarHearBeat()
{
    SENSOR_HEARTBEAT_t Lidar_HeartBeat = {0};
    can_msg_t can_msg = {0};
    Lidar_HeartBeat.SENSOR_HEARTBEAT_signal = 'a';
    dbc_msg_hdr_t msg_hdr =  dbc_encode_SENSOR_HEARTBEAT(can_msg.data.bytes, &Lidar_HeartBeat);
    can_msg.msg_id = msg_hdr.mid;
    can_msg.frame_fields.data_len = msg_hdr.dlc;
    CAN_tx(can_bus_number, &can_msg, 0);
}

//Transmits the obstacle information obtained from the LIDAR to the master
void SensorControllerCAN_Tx::CANTxLidarObstacleInfo(RPLidarRotation *singleRotation)
{
    SENSOR_LIDAR_OBSTACLE_INFO_t Lidar_info = {0};
    Lidar_info.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR0 = singleRotation->sector0;
    Lidar_info.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR1 = singleRotation->sector1;
    Lidar_info.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR2 = singleRotation->sector2;
    Lidar_info.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR3 = singleRotation->sector3;
    Lidar_info.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR4 = singleRotation->sector4;
    Lidar_info.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR5 = singleRotation->sector5;
    Lidar_info.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR6 = singleRotation->sector6;
    Lidar_info.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR7 = singleRotation->sector7;
    Lidar_info.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR8 = singleRotation->sector8;
    Lidar_info.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR9 = singleRotation->sector9;
    Lidar_info.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR10 = singleRotation->sector10;
    Lidar_info.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR11 = singleRotation->sector11;

    can_msg_t can_msg = {0};
    dbc_msg_hdr_t msg_hdr =  dbc_encode_SENSOR_LIDAR_OBSTACLE_INFO(can_msg.data.bytes, &Lidar_info);
    can_msg.msg_id = msg_hdr.mid;
    can_msg.frame_fields.data_len = msg_hdr.dlc;
    CAN_tx(can_bus_number, &can_msg, 2);
}
