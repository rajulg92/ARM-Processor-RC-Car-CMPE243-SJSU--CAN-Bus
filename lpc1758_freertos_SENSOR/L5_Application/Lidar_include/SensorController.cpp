/*
 * SensorController.cpp
 *
 *  Created on: Oct 28, 2017
 *      Author: Harshitha
 */

#include <Lidar_include/SensorController.hpp>
#include "LidarInterfacing.hpp"
#include "SensorControllerCAN_Tx.hpp"
#include "lpc_pwm.hpp"
#include "printf_lib.h"
#include "lpc_sys.h"
#include "error_stat_display.hpp"
#include <string.h>
static PWM pwm1(PWM::pwm2, 25500);
static RPLidar rpLidar;
static RPLidarRotation singleRotation;
static rplidar_response_measurement_node_t raw_response_node;
static RPLidarMeasurement refined_response_buff[360];
static RPLidarMeasurement refined_response;
static SensorControllerCAN_Tx sensorControllerCanTx;

//Initializes the UART and gets the healthinfo of the LIDAR. Rests the LIDAR if an invalid
//response is received.
void SensorController::InitCommunicationLidar()
{
    rpLidar.u3.init(rpLidar.RPLIDAR_SERIAL_BAUDRATE,11520,11520);
    rplidar_response_device_health_t info;
    int current_time = sys_get_uptime_ms();
    int Timeout = 2000;
    while(!IS_OK(rpLidar.getHealth(info, rpLidar.RPLIDAR_DEFAULT_TIMEOUT)) && Timeout < (int)(sys_get_uptime_ms() - current_time))
    {
        ErrorStatDisplay::displayErrorStat(kErrorStatDisplay_LidarFault);
        rpLidar.reset();
        vTaskDelay(200);
    }
    pwm1.set(0);
    pwm1.set(100);
    if(IS_OK(rpLidar.startScan(0,rpLidar.RPLIDAR_DEFAULT_TIMEOUT))){
        ErrorStatDisplay::clearErrorStat(kErrorStatDisplay_LidarFault);
        u0_dbg_printf("LIDAR start scan success\n");
    }
}

//Collects data corresponding to one rotation of the LIDAR and store it in a buffer.
void SensorController::CollectDataFromLidar()
{
    static int count = 0;
    count++;
    for(int i=0;i<360;i++)
    {
    	memset(&(refined_response_buff[i]), 0, sizeof(RPLidarMeasurement));
        if(IS_OK(rpLidar.waitPoint(&raw_response_node, rpLidar.RPLIDAR_DEFAULT_TIMEOUT, &refined_response)))
            refined_response_buff[i] = refined_response;
    }

    //TODO; why we need to reset?
    if(count%10 == 0)
    {
        xQueueReset(rpLidar.u3.getRxQueue());
        count = 0;
    }
}

//Divides the the reading obtained from the LIDAR into sectors and tracks.
void SensorController::ProcessTheObtainedData()
{
    int length = (sizeof(refined_response_buff))/(sizeof(refined_response_buff[0]));
    //memset(&singleRotation, 0, sizeof(singleRotation));
#if 1
    int* p = (int*)&singleRotation;
    for(int i = 0; i < 12; i++)
    {
        *p = 13;
        p++;
    }
#endif
    rpLidar.sectorSelection(refined_response_buff,length, &singleRotation);
}

//Transmits the processed data over CAN bus
void SensorController::TransmitDataOverCAN()
{
    sensorControllerCanTx.CANTxLidarObstacleInfo(&singleRotation);
}
