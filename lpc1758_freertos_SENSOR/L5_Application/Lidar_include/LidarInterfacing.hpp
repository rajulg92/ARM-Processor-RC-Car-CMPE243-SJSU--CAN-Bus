/*
 * LidarInterfacing.hpp
 *
 *  Created on: 23-Oct-2017
 *      Author: Sushma, Harshitha, Supradeep
 */
#ifndef LIDARINTERFACING_HPP_
#define LIDARINTERFACING_HPP_

#include <Lidar_include/LidarCommands.hpp>
#include "uart3.hpp"

#define SECTOR0_BOUNDARY  (20.0)
#define SECTOR1_BOUNDARY  (SECTOR0_BOUNDARY + 25.0)
#define SECTOR2_BOUNDARY  (SECTOR1_BOUNDARY + 15.0)
#define SECTOR3_START     (90.0)
#define SECTOR3_BOUNDARY  (SECTOR3_START + 30.0)
#define SECTOR4_BOUNDARY  (SECTOR3_BOUNDARY + 30.0)
#define SECTOR5_BOUNDARY  (SECTOR4_BOUNDARY + 30.0)
#define SECTOR6_BOUNDARY  (SECTOR5_BOUNDARY + 30.0)
#define SECTOR7_BOUNDARY  (SECTOR6_BOUNDARY + 30.0)
#define SECTOR8_BOUNDARY  (SECTOR7_BOUNDARY + 30.0)
#define SECTOR9_START     (300.0)
#define SECTOR9_BOUNDARY  (SECTOR9_START + 15.0)
#define SECTOR10_BOUNDARY (SECTOR9_BOUNDARY + 25.0)
#define SECTOR11_BOUNDARY (SECTOR10_BOUNDARY + 20.0)
#if 0
#define SECTOR0_BOUNDARY  (10.0)
#define SECTOR1_BOUNDARY  (SECTOR0_BOUNDARY + 10.0)
#define SECTOR2_BOUNDARY  (SECTOR1_BOUNDARY + 25.0)
#define SECTOR3_START     (90.0)
#define SECTOR3_BOUNDARY  (SECTOR3_START + 30.0)
#define SECTOR4_BOUNDARY  (SECTOR3_BOUNDARY + 30.0)
#define SECTOR5_BOUNDARY  (SECTOR4_BOUNDARY + 30.0)
#define SECTOR6_BOUNDARY  (SECTOR5_BOUNDARY + 30.0)
#define SECTOR7_BOUNDARY  (SECTOR6_BOUNDARY + 30.0)
#define SECTOR8_BOUNDARY  (SECTOR7_BOUNDARY + 30.0)
#define SECTOR9_START     (315.0)
#define SECTOR9_BOUNDARY  (SECTOR9_START + 25.0)
#define SECTOR10_BOUNDARY (SECTOR9_BOUNDARY + 10.0)
#define SECTOR11_BOUNDARY (SECTOR10_BOUNDARY + 10.0)
#endif
#if 0
#define SECTOR0_BOUNDARY  (10.0)
#define SECTOR1_BOUNDARY  (SECTOR0_BOUNDARY + 10.0)
#define SECTOR2_BOUNDARY  (SECTOR1_BOUNDARY + 10.0)
#define SECTOR3_START     (90.0)
#define SECTOR3_BOUNDARY  (SECTOR3_START + 30.0)
#define SECTOR4_BOUNDARY  (SECTOR3_BOUNDARY + 30.0)
#define SECTOR5_BOUNDARY  (SECTOR4_BOUNDARY + 30.0)
#define SECTOR6_BOUNDARY  (SECTOR5_BOUNDARY + 30.0)
#define SECTOR7_BOUNDARY  (SECTOR6_BOUNDARY + 30.0)
#define SECTOR8_BOUNDARY  (SECTOR7_BOUNDARY + 30.0)
#define SECTOR9_START     (270.0 + 60.0)
#define SECTOR9_BOUNDARY  (SECTOR9_START + 10.0)
#define SECTOR10_BOUNDARY (SECTOR9_BOUNDARY + 10.0)
#define SECTOR11_BOUNDARY (SECTOR10_BOUNDARY + 10.0)
#endif

struct RPLidarMeasurement
{
    float distance = 0;
    float angle = 0;
    uint8_t quality = 0;
    bool  startBit = 0;
};

typedef struct RPLidarrotation
{
    int sector0;
    int sector1;
    int sector2;
    int sector3;
    int sector4;
    int sector5;
    int sector6;
    int sector7;
    int sector8;
    int sector9;
    int sector10;
    int sector11;
} RPLidarRotation;

class RPLidar
{
    public:
        enum{
            RPLIDAR_SERIAL_BAUDRATE = 115200,
            RPLIDAR_DEFAULT_TIMEOUT = 500,
        };

        //Uart3 for Lidar communication
        Uart3 &u3 = Uart3::getInstance();

        //Ask the RPLIDAR for its device info like the serial number
        uint32_t getDeviceInfo(rplidar_response_device_info_t & info, uint32_t timeout = RPLIDAR_DEFAULT_TIMEOUT);

        //Ask the RPLIDAR for its health info
        uint32_t getHealth(rplidar_response_device_health_t & healthinfo, uint32_t timeout = RPLIDAR_DEFAULT_TIMEOUT);

        //Stops the LIDAR scan
        uint32_t stop();

        //Resets the LIDAR
        uint32_t reset();

        //Starts the LIDAR scan
        uint32_t startScan(bool force = false, uint32_t timeout = RPLIDAR_DEFAULT_TIMEOUT*2);

        //Waits for one sample point to arrive
        uint32_t waitPoint(rplidar_response_measurement_node_t *node,uint32_t timeout, struct RPLidarMeasurement *measurement);

        //Selects the track for the sample point
        int trackSelection(float distance);

        //Selects the sector for the sample point
        void sectorSelection(RPLidarMeasurement *measurement_buff,int length, struct RPLidarrotation *rot);

    protected:
        //Sends a command to the LIDAR
        uint32_t _sendCommand(uint8_t cmd, const void * payload, size_t payloadsize);

        //Receives a the response from the Lidar
        uint32_t _waitResponseHeader(rplidar_ans_header_t * header, uint32_t timeout);

};

#endif /* LIDARINTERFACING_HPP_ */
