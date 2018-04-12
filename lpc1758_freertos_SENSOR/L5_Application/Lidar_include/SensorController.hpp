/*
 * SensorController.hpp
 *
 *  Created on: Oct 28, 2017
 *      Author: Harshitha, Sushma, Supradeep
 */

#ifndef SENSORCONTROLLER_HPP_
#define SENSORCONTROLLER_HPP_

class SensorController{
    public:

        //Initializes the UART and gets the healthinfo of the LIDAR. Rests the LIDAR if an invalid
        //response is received.
        void InitCommunicationLidar(void);

        //Collects data corresponding to one rotation of the LIDAR and store it in a buffer.
        void CollectDataFromLidar(void);

        //Divides the the reading obtained from the LIDAR into sectors and tracks.
        void ProcessTheObtainedData(void);

        //Transmits the processed data over CAN bus
        void TransmitDataOverCAN();
};

#endif /* SENSORCONTROLLER_HPP_ */
