/*
 * LidarInterfacing.cpp
 *
 *  Created on: 23-Oct-2017
 *      Author: Sushma, Harshitha, Supradeep
 */

#include <Lidar_include/LidarInterfacing.hpp>
#include <stddef.h>
#include "lpc_sys.h"
#include "utilities.h"
#include "printf_lib.h"
#include <stdint.h>
#include "error_stat_display.hpp"
#define LidarRange_mm 3000
//13 indicates an invalid track
static int previous_track[12] = {13};

//Sends a command to the LIDAR
uint32_t RPLidar::_sendCommand(uint8_t cmd, const void * payload, size_t payloadsize)
{
    rplidar_cmd_packet_t pkt_header;
    rplidar_cmd_packet_t * header = &pkt_header;

    if (payloadsize && payload) {
        cmd |= RPLIDAR_CMDFLAG_HAS_PAYLOAD;
    }

    header->syncByte = RPLIDAR_CMD_SYNC_BYTE;
    header->cmd_flag = cmd;
    u3.putChar(header->syncByte, portMAX_DELAY);
    u3.putChar(header->cmd_flag, portMAX_DELAY);
    return RESULT_OK;
}

//Receives a response from the LIDAR
uint32_t RPLidar::_waitResponseHeader(rplidar_ans_header_t * header, uint32_t timeout)
{
    uint8_t  recvPos = 0;
    uint32_t currentTs = sys_get_uptime_ms();
    uint32_t remainingtime;
    uint8_t *headerbuf = (uint8_t*)header;
    while ((remainingtime=sys_get_uptime_ms() - currentTs) <= timeout) {

        char recieved_char;
        u3.getChar(&recieved_char, portMAX_DELAY);
        int currentbyte = recieved_char;
        if (currentbyte<0) continue;
        switch (recvPos) {
        case 0:
            if (currentbyte != RPLIDAR_ANS_SYNC_BYTE1) {
                continue;
            }
            break;
        case 1:
            if (currentbyte != RPLIDAR_ANS_SYNC_BYTE2) {
                recvPos = 0;
                continue;
            }
            break;
        }
        headerbuf[recvPos++] = currentbyte;

        if (recvPos == sizeof(rplidar_ans_header_t)) {
            return RESULT_OK;
        }
    }
    return RESULT_OPERATION_TIMEOUT;
}

// Ask the RPLIDAR for its device
uint32_t RPLidar::getDeviceInfo(rplidar_response_device_info_t & info, uint32_t timeout )
{

    uint8_t  recvPos = 0;
    uint32_t currentTs = sys_get_uptime_ms();
    uint32_t remainingtime;
    uint8_t *infobuf = (uint8_t*)&info;
    rplidar_ans_header_t response_header;
    uint32_t  ans;

        if (IS_FAIL(ans = _sendCommand(RPLIDAR_CMD_GET_DEVICE_INFO,NULL,0))) {
            return ans;
        }

        if (IS_FAIL(ans = _waitResponseHeader(&response_header, timeout))) { // timeout is same as device function timeout
            return ans;
        }

        // verify whether we got a correct header
        if (response_header.type != RPLIDAR_ANS_TYPE_DEVINFO) {
            return RESULT_INVALID_DATA;
        }

        if (response_header.size < sizeof(rplidar_response_device_info_t)) {
            return RESULT_INVALID_DATA;
        }
        while ((remainingtime=sys_get_uptime_ms() - currentTs) <= timeout) {
            char recieved_char;
            u3.getChar(&recieved_char, portMAX_DELAY);
            int currentbyte = recieved_char;
            if (currentbyte<0) continue;
            infobuf[recvPos++] = currentbyte;
            if (recvPos == sizeof(rplidar_response_device_info_t)) {
                return RESULT_OK;
            }
        }

    return RESULT_OPERATION_TIMEOUT;
}

// Ask the RPLIDAR for its health info
uint32_t RPLidar::getHealth(rplidar_response_device_health_t & healthinfo, uint32_t timeout)
{
    uint32_t currentTs = sys_get_uptime_ms();
    uint32_t remainingtime;

    uint8_t *infobuf = (uint8_t *)&healthinfo;
    uint8_t recvPos = 0;

    rplidar_ans_header_t response_header;
    uint32_t  ans;

    if (IS_FAIL(ans = _sendCommand(RPLIDAR_CMD_GET_DEVICE_HEALTH, NULL, 0))) {
        return ans;
    }

    if (IS_FAIL(ans = _waitResponseHeader(&response_header, timeout))) {
        return ans;
    }

    // verify whether we got a correct header

    if (response_header.type != RPLIDAR_ANS_TYPE_DEVHEALTH) {
        return RESULT_INVALID_DATA;
    }

    if ((response_header.size) < sizeof(rplidar_response_device_health_t)) {
        return RESULT_INVALID_DATA;
    }
    while ((remainingtime=sys_get_uptime_ms() - currentTs) <= timeout) {
        char recieved_char;
        u3.getChar(&recieved_char, portMAX_DELAY);
        int currentbyte = recieved_char;
        if (currentbyte < 0) continue;

        infobuf[recvPos++] = currentbyte;
        if (recvPos == sizeof(rplidar_response_device_health_t)) {
            return RESULT_OK;
        }
    }
    return RESULT_OPERATION_TIMEOUT;
}

//Stops the LIDAR scan operation
uint32_t RPLidar::stop()
{
    uint32_t ans = _sendCommand(RPLIDAR_CMD_STOP,NULL,0);
    return ans;
}

//Resets the LIDAR
uint32_t RPLidar::reset()
{
    uint32_t ans = _sendCommand(RPLIDAR_CMD_RESET,NULL,0);
    return ans;
}

//Starts the LIDAR scan
uint32_t RPLidar::startScan(bool force, uint32_t timeout)
{
    uint32_t ans;
    stop(); //force the previous operation to stop

    {
        ans = _sendCommand(force?RPLIDAR_CMD_FORCE_SCAN:RPLIDAR_CMD_SCAN, NULL, 0);
        if (IS_FAIL(ans)) return ans;

        // waiting for confirmation
        rplidar_ans_header_t response_header;
        if (IS_FAIL(ans = _waitResponseHeader(&response_header, timeout))) {
            return ans;
        }

        // verify whether we got a correct header
        if (response_header.type != RPLIDAR_ANS_TYPE_MEASUREMENT) {
            return RESULT_INVALID_DATA;
        }

        if (response_header.size < sizeof(rplidar_response_measurement_node_t)) {
            return RESULT_INVALID_DATA;
        }
    }
    return RESULT_OK;
}

//Waits for one sample point to arrive
uint32_t RPLidar::waitPoint(rplidar_response_measurement_node_t *node, uint32_t timeout, RPLidarMeasurement *measurement)
{
    uint32_t currentTs = sys_get_uptime_ms();
    uint32_t remainingtime;
    uint8_t *nodebuf = (uint8_t*)node;
    uint8_t recvPos = 0;
   while ((remainingtime=sys_get_uptime_ms() - currentTs) <= timeout) {
       char recieved_char;
       int currentbyte = 0;
       if(u3.getChar(&recieved_char,portMAX_DELAY)){
           currentbyte = recieved_char;
       }
        if (currentbyte<0) continue;

        switch (recvPos) {
            case 0: // expect the sync bit and its reverse in this byte          {
                {
                    uint8_t tmp = (currentbyte>>1);
                    if ( (tmp ^ currentbyte) & 0x1 ) {
                        // pass
                    } else {
                        continue;
                    }

                }
                break;
            case 1: // expect the highest bit to be 1
                {
                    if (currentbyte & RPLIDAR_RESP_MEASUREMENT_CHECKBIT) {
                        // pass
                    } else {
                        recvPos = 0;
                        continue;
                    }
                }
                break;
          }
          nodebuf[recvPos++] = currentbyte;

          if (recvPos == sizeof(rplidar_response_measurement_node_t)) {
              measurement->distance = node->distance_q2/(float)4.0;
              measurement->angle = (node->angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT)/(float)64.0;
              measurement->quality = (node->sync_quality>>RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT);
              measurement->startBit = (node->sync_quality & RPLIDAR_RESP_MEASUREMENT_SYNCBIT);
              ErrorStatDisplay::clearErrorStat(kErrorStatDisplay_NoData);
              return RESULT_OK;
          }
   }
   ErrorStatDisplay::displayErrorStat(kErrorStatDisplay_NoData);
   return RESULT_OPERATION_TIMEOUT;
}

//Selects the track for the sample point
int RPLidar::trackSelection(float distance){

    int track;
    int track_range = LidarRange_mm/12;
    if(distance > 0 && distance<= track_range ){
        track = 1;
    }
    else if(distance > track_range && distance <= track_range*2){
        track = 2;
    }
    else if(distance > track_range*2 && distance <= track_range*3){
        track = 3;
    }
    else if(distance > track_range*3 && distance <= track_range*4){
        track = 4;
    }
    else if(distance > track_range*4 && distance <= track_range*5){
        track = 5;
    }
    else if(distance > track_range*5 && distance <= track_range*6){
        track = 6;
    }
    else if(distance > track_range*6 && distance <= track_range*7){
        track = 7;
    }
    else if(distance > track_range*7 && distance <= track_range*8){
        track = 8;
    }
    else if(distance > track_range*8 && distance <= track_range*9){
        track = 9;
    }
    else if(distance > track_range*9 && distance <= track_range*10){
        track = 10;
    }
    else if(distance > track_range*10 && distance <= track_range*11){
        track = 11;
    }
    else if(distance > track_range*11 && distance <= track_range*12){
        track = 12;
    }
    else{
        track = 13;
    }
    return track;
}

//Selects the sector for the sample point
void RPLidar::sectorSelection(RPLidarMeasurement *measurement_buff,int length, struct RPLidarrotation *rot)
{

    for(int j=0;j<12;j++)
    {
            previous_track[j] = 13;
    }
    for(int i=0; i<length; i++)
    {
      int track = 13;
      if(measurement_buff[i].quality!=0){
          if(measurement_buff[i].angle >= 0.0 && measurement_buff[i].angle <= SECTOR0_BOUNDARY)
                 {
                     track = trackSelection(measurement_buff[i].distance);
                     rot->sector0 = previous_track[0];
                     if (track < previous_track[0]) rot->sector0 = track;
                     previous_track[0] = track;
                 }
                 else if(measurement_buff[i].angle > SECTOR0_BOUNDARY && measurement_buff[i].angle <= SECTOR1_BOUNDARY)
                   {
                      track = trackSelection(measurement_buff[i].distance);
                      rot->sector1 = previous_track[1];
                       if (track < previous_track[1]) rot->sector1 = track;
                       previous_track[1] = track;
                   }
                   else if(measurement_buff[i].angle > SECTOR1_BOUNDARY && measurement_buff[i].angle <= SECTOR2_BOUNDARY)
                   {
                       track = trackSelection(measurement_buff[i].distance);
                       rot->sector2 = previous_track[2];
                       if (track < previous_track[2]) rot->sector2 = track;
                       previous_track[2] = track;
                   }
                   else if(measurement_buff[i].angle > SECTOR3_START && measurement_buff[i].angle <= SECTOR3_BOUNDARY)
                   {
                       track = trackSelection(measurement_buff[i].distance);
                       rot->sector3 = previous_track[3];
                       if (track < previous_track[3]) rot->sector3 = track;
                       previous_track[3] = track;
                   }
                   else if(measurement_buff[i].angle > SECTOR3_BOUNDARY && measurement_buff[i].angle <= SECTOR4_BOUNDARY)
                   {
                       track = trackSelection(measurement_buff[i].distance);
                       rot->sector4 = previous_track[4];
                       if (track < previous_track[4]) rot->sector4 = track;
                       previous_track[4] = track;
                   }
                   else if(measurement_buff[i].angle > SECTOR4_BOUNDARY && measurement_buff[i].angle <= SECTOR5_BOUNDARY)
                   {
                       track = trackSelection(measurement_buff[i].distance);
                       rot->sector5 = previous_track[5];
                       if (track < previous_track[5]) rot->sector5 = track;
                       previous_track[5] = track;
                   }
                   else if(measurement_buff[i].angle > SECTOR5_BOUNDARY && measurement_buff[i].angle <= SECTOR6_BOUNDARY)
                   {
                       track = trackSelection(measurement_buff[i].distance);
                       rot->sector6 = previous_track[6];
                       if (track < previous_track[6]) rot->sector6 = track;
                       previous_track[6] = track;
                   }
                   else if(measurement_buff[i].angle > SECTOR6_BOUNDARY && measurement_buff[i].angle <= SECTOR7_BOUNDARY)
                   {
                       track = trackSelection(measurement_buff[i].distance);
                       rot->sector7 = previous_track[7];
                       if (track < previous_track[7]) rot->sector7 = track;
                       previous_track[7] = track;
                   }
                   else if(measurement_buff[i].angle > SECTOR7_BOUNDARY && measurement_buff[i].angle <= SECTOR8_BOUNDARY)
                   {
                       track = trackSelection(measurement_buff[i].distance);
                       rot->sector8 = previous_track[8];
                       if (track < previous_track[8]) rot->sector8 = track;
                       previous_track[8] = track;
                   }
                   else if(measurement_buff[i].angle > SECTOR9_START && measurement_buff[i].angle <= SECTOR9_BOUNDARY)
                   {
                       track = trackSelection(measurement_buff[i].distance);
                       rot->sector9 = previous_track[9];
                       if (track < previous_track[9]) rot->sector9 = track;
                       previous_track[9] = track;
                   }
                   else if(measurement_buff[i].angle > SECTOR9_BOUNDARY && measurement_buff[i].angle <= SECTOR10_BOUNDARY)
                   {
                       track = trackSelection(measurement_buff[i].distance);
                       rot->sector10 = previous_track[10];
                       if (track < previous_track[10]) rot->sector10 = track;
                       previous_track[10] = track;
                   }
                   else if(measurement_buff[i].angle > SECTOR10_BOUNDARY && measurement_buff[i].angle <= SECTOR11_BOUNDARY)
                   {
                       track = trackSelection(measurement_buff[i].distance);
                       rot->sector11 = previous_track[11];
                       if (track < previous_track[11]) rot->sector11 = track;
                       previous_track[11] = track;
                   }
               }
    }
}




