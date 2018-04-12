/*
 * LidarTask.cpp
 *
 *  Created on: Oct 28, 2017
 *      Author: Harshitha, Sushma, Supradeep
 */

#include <Lidar_include/SensorController.hpp>
#include "LidarTask.hpp"
#include "scheduler_task.hpp"

static SensorController sensorController;

//Constructor
LIDARtask::LIDARtask(uint8_t priority):scheduler_task("LIDARtask", 2000, priority){
    //Nothing to Initialize
}

//Initialize communication with LIDAR
bool LIDARtask::init(void){
    sensorController.InitCommunicationLidar();
    return true;
}

//Continuously collect data from the LIDAR and process it
bool LIDARtask::run(void *p){
    sensorController.CollectDataFromLidar();
    sensorController.ProcessTheObtainedData();
    sensorController.TransmitDataOverCAN();
    return true;
}
