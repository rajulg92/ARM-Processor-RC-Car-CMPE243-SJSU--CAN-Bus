/****************************************************************************
 * sensor_io_interface.cpp
 * @file : This file implements the Interface class methods to handle sensor
 *         detected obstacles.
 * @created : Oct 16,2017
 *
 ****************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include "../../_can_dbc/generated_can.h"
#include <master/sensor_io_interface.hpp>


//#define VERBOSE_E
//#define DEBUG_E
#include "debug.h"

ISensorController::ISensorController(): LidarObstacleMsg({0}), mcurrent_obstacle_status(obs_clear),mobs_detection({0})
{
    LOGD("ISensorController Object Instantiated");
}

ISensorController::obs_detection_t ISensorController::RunObstacleDetectionAlgo()
{
    ISensorController::obs_detection_t obs_detection ={0};
    obs_detection.no_obstacle = true;
    obs_detection.front =  false;
    obs_detection.rear =  false;
    uint8_t obs_track = 6;
    uint8_t clearance_track = 5;


    //front
    if(((LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR0 > 0)  &&
        (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR0 < obs_track+1)) ||
       ((LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR11 > 0) &&
        (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR11 < obs_track+1)))
    {

       obs_detection.no_obstacle = false;
       obs_detection.front =  true;

    }

    //front left
    if(((LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR10 > 0) &&
        (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR10 < obs_track+1)) ||

       (( (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR9 > 0) &&
          (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR9 < obs_track)
         ) &&

         (((LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR10 > 0) &&
           (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR10 < obs_track+1)) ||
          ((LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR11 > 0) &&
           (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR11 < obs_track+1))))
           )
        {
           obs_detection.frontleft = true;
           obs_detection.no_obstacle = false;

        }


    //front right
    if(((LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR1 > 0) &&
        (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR1 < obs_track+1)) ||

       (((LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR2 > 0) &&
         (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR2 < obs_track)) &&
        (((LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR0 > 0) &&
          (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR0 < obs_track+1)) ||
         ((LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR1 > 0) &&
          (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR1 < obs_track+1))))
      )
    {
        obs_detection.frontright = true;
        obs_detection.no_obstacle = false;

    }

    //Rear
    if(((LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR3 > 0) &&
        (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR3 < obs_track)) ||
       ((LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR4 > 0) &&
        (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR4 < obs_track)))
    {
         obs_detection.rearright = true;
         obs_detection.no_obstacle = false;

     }

    //Rear right
    if(((LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR5 > 0) &&
        (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR5 < obs_track)) ||
       ((LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR6 > 0) &&
        (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR6 < obs_track)))
    {
        obs_detection.rear = true;
        obs_detection.no_obstacle = false;
    }


    //rear left
    if(((LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR7 > 0) &&
        (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR7 < obs_track)) ||
       ((LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR8 > 0) &&
        (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR8 < obs_track)))
    {
        obs_detection.rearleft= true;
        obs_detection.no_obstacle = false;

    }

    //front clearance checks:
    //for front right
    if((obs_detection.front
            && (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR0 < clearance_track
                || LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR11 < clearance_track))
       && !obs_detection.frontright)
    {
        obs_detection.frontright = true;
        obs_detection.no_obstacle = false;
    }

    //for front left
    if((obs_detection.front
            && (LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR0 < clearance_track
                || LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR11 < clearance_track))
       && !obs_detection.frontleft)
    {
        obs_detection.frontleft = true;
        obs_detection.no_obstacle = false;
    }

    set_obstacle_detection(obs_detection);
    return obs_detection;
}

void ISensorController::set_obstacle_detection(obs_detection_t adetections)
{
    mobs_detection.frontleft = adetections.frontleft;
    mobs_detection.frontright = adetections.frontright;
    mobs_detection.rearleft = adetections.rearleft;
    mobs_detection.rearright = adetections.rearright;
    mobs_detection.no_obstacle = adetections.no_obstacle;
}














/****************************************************************************
 * Revision History
 * @format : version/ Date / Author / Change description
 ****************************************************************************
 * Version  Date         Author    Change Log
 * 1        10-16-2017   Revathy   Created Initial file
 * 2        10-27-2017   Revathy   Implemented ObstacleDetectionAlgorithm
 ****************************************************************************/
