/**********************************************************
 * test_master_controller.cpp
 *
 * @file : Unit tests for master_controller.cpp
 *  Created on: Nov 13, 2017
 *
 **********************************************************/
#include "sensor_io_interface.hpp"

static ISensorController* psensor = new ISensorController;
static ISensorController::obs_detection_t exp_detections = {0};

void mock_CAN_Rx_Lidar_Info(uint8_t sec0,uint8_t sec11,
                            uint8_t sec1,uint8_t sec2,
                            uint8_t sec9,uint8_t sec10,
                            uint8_t sec5,uint8_t sec6,
                            uint8_t sec3,uint8_t sec4,
                            uint8_t sec7,uint8_t sec8);

void set_expected_detection(bool afront,bool afr, bool afl,
                            bool arear,bool arr, bool arl,
                            bool aclear);

void mock_CAN_Rx_Lidar_Info(uint8_t sec0,
                            uint8_t sec11,
                            uint8_t sec1,
                            uint8_t sec2,
                            uint8_t sec9,
                            uint8_t sec10,
                            uint8_t sec5,
                            uint8_t sec6,
                            uint8_t sec3,
                            uint8_t sec4,
                            uint8_t sec7,
                            uint8_t sec8)
{
    psensor->LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR0 =sec0;
    psensor->LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR1 =sec1;
    psensor->LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR2 =sec2;
    psensor->LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR3 =sec3;
    psensor->LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR4 =sec4;
    psensor->LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR5 =sec5;
    psensor->LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR6 =sec6;
    psensor->LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR7 =sec7;
    psensor->LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR8 =sec8;
    psensor->LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR9 =sec9;
    psensor->LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR10 =sec10;
    psensor->LidarObstacleMsg.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR11 =sec11;
}

void set_expected_detection(bool afront,bool afr, bool afl,
                            bool arear,bool arr, bool arl,
                            bool aclear)
{
    exp_detections.front = afront;
    exp_detections.frontleft = afl;
    exp_detections.frontright = afr;
    exp_detections.no_obstacle = aclear;
    exp_detections.rear = arear;
    exp_detections.rearleft = arl;
    exp_detections.rearright = arr;
}

int compare_detections(ISensorController::obs_detection_t adetections)
{
    int ret=0;
    switch(ret)
    {
        case 0:
        case 1:
            if (exp_detections.front == adetections.front){++ret;}
            else{ return ret; } //1
        case 2:
            if (exp_detections.frontright == adetections.frontright){++ret;}
            else{ return ret; } //2
        case 3:
            if (exp_detections.frontleft == adetections.frontleft){++ret;}
            else{ return ret; }
        case 4:
            if (exp_detections.rear == adetections.rear){++ret;}
            else{ return ret; }
        case 5:
            if (exp_detections.rearright == adetections.rearright){++ret;}
            else{ return ret; }
        case 6:
            if (exp_detections.rearleft == adetections.rearleft){++ret;}
            else{ return ret; }
        case 7:
            if (exp_detections.no_obstacle == adetections.no_obstacle){++ret;}
            else{ return ret; }
        default :
            return ret; //7
    }
}

Ensure(test_obstacle_detection)
{
    // detection flag argument sequence - f , fr , fl ,r , rr , rl ,clear
    // sector argument sequence - 0 , 11 ,1 , 2, 9, 10, 5, 6, 3, 4, 7, 8
    ISensorController::obs_detection_t actual_detections = {0};

    mock_CAN_Rx_Lidar_Info(2,2,6,0,2,2,4,0,2,0,5,0);
    set_expected_detection(true,false,true,false,true,false,false);
    actual_detections = psensor->RunObstacleDetectionAlgo();
    assert_that(compare_detections(actual_detections) , is_equal_to(7));

    mock_CAN_Rx_Lidar_Info(3,3,2,12,5,12,6,12,1,12,1,12);
    set_expected_detection(false,true,false,false,true,true,false);
    actual_detections = psensor->RunObstacleDetectionAlgo();
    assert_that(compare_detections(actual_detections) , is_equal_to(7));

    mock_CAN_Rx_Lidar_Info(1,4,6,2,1,0,0,7,0,3,0,1);
    set_expected_detection(true,true,false,false,false,true,false);
    actual_detections = psensor->RunObstacleDetectionAlgo();
    assert_that(compare_detections(actual_detections) , is_equal_to(7));

    mock_CAN_Rx_Lidar_Info(0,0,0,0,0,0,0,0,0,0,0,0);
    set_expected_detection(false,false,false,false,false,false,true);
    actual_detections = psensor->RunObstacleDetectionAlgo();
    assert_that(compare_detections(actual_detections) , is_equal_to(7));

    mock_CAN_Rx_Lidar_Info(4,4,4,4,4,4,4,4,4,4,4,4);
    set_expected_detection(false,false,false,false,false,false,true);
    actual_detections = psensor->RunObstacleDetectionAlgo();
    assert_that(compare_detections(actual_detections) , is_equal_to(7));

    mock_CAN_Rx_Lidar_Info(2,2,2,2,2,2,4,4,4,4,4,4);
    set_expected_detection(true,true,true,false,false,false,false);
    actual_detections = psensor->RunObstacleDetectionAlgo();
    assert_that(compare_detections(actual_detections) , is_equal_to(7));


}

TestSuite* sensor_controller_suite(void)
{
    TestSuite* sensor_suite = create_test_suite();
    add_test(sensor_suite,test_obstacle_detection);

    return sensor_suite;
}
