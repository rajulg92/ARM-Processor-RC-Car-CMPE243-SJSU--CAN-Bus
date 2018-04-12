/*
 * SensorControllerCAN_Tx.hpp
 *
 *  Created on: Oct 28, 2017
 *      Author: Harshitha
 */

#ifndef SENSORCONTROLLERCAN_TX_HPP_
#define SENSORCONTROLLERCAN_TX_HPP_
#include "LidarInterfacing.hpp"
#include "can.h"

class SensorControllerCAN_Tx{
    public:

        //Initializes the CAN bus
        void InitLidarCAN();

        //Resets the CAN bus if it is off
        void CANTxLidarObstacleInfo(RPLidarRotation *singleRotation);

        //Transmits the Heart beat
        void CanTxLidarHearBeat(void);

        //Transmits the obstacle information obtained from the LIDAR to the master
        void CanTxLidarBusOffAction(void);

        can_t can_bus_number = can2;
};



#endif /* SENSORCONTROLLERCAN_TX_HPP_ */
