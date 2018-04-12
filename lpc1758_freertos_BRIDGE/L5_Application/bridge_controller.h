#ifndef __BRIDGE_H_
#define __BRIDGE_H_
#include <stdbool.h>
#include <stdint.h>
#include "../_can_dbc/generated_can.h"
extern "C"
{

/**
 * GEO_TELEMETRY_LOCK
 * 1) GPS lock status
 *
 * GEO_TELECOMPASS:
 *
 * 1) compass (heading)
 * 2) bearing
 * 3) distance to destination,
 * 4) destination reached status
 * 5) current checkpoint ID
 *
 * GEO_TURNING_ANGLE:
 * 1) Turning angle
 * */
#define JSON_ID_COMPASS_HEADING "heading"
#define JSON_ID_COMPASS_BEARING "bearing"
#define JSON_ID_TURNING_ANGLE   "turning"
#define JSON_ID_DIST_TO_DEST    "dist_to_dest"
#define JSON_ID_DEST_REACHED    "dest_reach_stat"
#define JSON_ID_CHECKPOINT_ID   "checkpoint_id"
#define JSON_ID_GPS_LOCK_STAT   "gps_lock"
#define JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC0    "lidar_sect0"
#define JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC1    "lidar_sect1"
#define JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC2    "lidar_sect2"
#define JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC3    "lidar_sect3"
#define JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC4    "lidar_sect4"
#define JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC5    "lidar_sect5"
#define JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC6    "lidar_sect6"
#define JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC7    "lidar_sect7"
#define JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC8    "lidar_sect8"
#define JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC9    "lidar_sect9"
#define JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC10   "lidar_sect10"
#define JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC11   "lidar_sect11"
#define JSON_ID_CHECKPOINTS_COUNT                  "gps_cp_count"
#define JSON_ID_CURR_LAT                           "gps_curr_lat"
#define JSON_ID_CURR_LONG                          "gps_curr_long"




static const char spcDEBUG_JSON_FORMAT[] = "{\r\n" \
        "        " JSON_ID_GPS_LOCK_STAT ":   %d,\r\n"  \
        "        " JSON_ID_COMPASS_HEADING ":   %f,\r\n"  \
        "        " JSON_ID_COMPASS_BEARING ":   %f,\r\n"  \
        "        " JSON_ID_TURNING_ANGLE ":   %ld,\r\n"  \
        "        " JSON_ID_DIST_TO_DEST ":   %f,\r\n"  \
        "        " JSON_ID_DEST_REACHED ":   %d,\r\n"  \
        "        " JSON_ID_CHECKPOINT_ID ":   %ld,\r\n"  \
        "        " JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC0 ":   %d,\r\n"  \
        "        " JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC1 ":   %d,\r\n"  \
        "        " JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC2 ":   %d,\r\n"  \
        "        " JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC3 ":   %d,\r\n"  \
        "        " JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC4 ":   %d,\r\n"  \
        "        " JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC5 ":   %d,\r\n"  \
        "        " JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC6 ":   %d,\r\n"  \
        "        " JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC7 ":   %d,\r\n"  \
        "        " JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC8 ":   %d,\r\n"  \
        "        " JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC9 ":   %d,\r\n"  \
        "        " JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC10 ":   %d,\r\n"  \
        "        " JSON_ID_SENSOR_LIDAR_OBSTACLE_INFO_SEC11 ":   %d,\r\n"  \
        "        " JSON_ID_CHECKPOINTS_COUNT ":   %ld,\r\n"  \
        "        " JSON_ID_CURR_LAT ":   %f,\r\n"  \
        "        " JSON_ID_CURR_LONG ":   %f\r\n"  \
        "}";

typedef struct
{
    float fLat;
    float fLong;
}tGPSLatLng;

typedef struct
{
    bool bGpsLock;
    double fCompassHeading;
    double fCompassBearing;
    int32_t nTurningAngle;
    double fDistToDest;
    bool bDestReachStat;
    uint32_t nCheckpointID;
    double fcurrLat;
    double fcurrLong;
    SENSOR_LIDAR_OBSTACLE_INFO_t lidar_obstacle;
}tBridgeCtrlrCtx;

typedef struct bt_data {
    char id;
    char data[8];
}bt_data_t;
void bt_ops_100Hz(void);
size_t bt_init(void);
void can_init(void);
void send_heartbeat_to_master(void);
//bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8]);

void turn_off_motor_request(void);
void turn_on_motor_request(void);
void send_gps_data(double,double);
void send_gps_data_count(int count);
void vHandleCANIncoming(size_t ahBrigdeCtrlrCtx);
void vSendDebugInfoToAndroid(size_t ahBrigdeCtrlrCtx);
void get_gps_locks(void);
void vSendHardcodedGPSCheckpoints(size_t ahBrigdeCtrlrCtx, int anPathIdx);
}
#endif
