#include <stdio.h>
#include <string.h>
#include "bridge_controller.h"
#include "uart3.hpp"
#include "GPIO.hpp"
#include "can.h"
#include "io.hpp"

//#define VERBOSE_E
#include "debug_e.h"


#define APP_RX_STACK_SIZE  (1024*10)
#define gps_coords_size (255)
char* token[gps_coords_size] = {NULL};
char* chckptlist[2*gps_coords_size] = {NULL};


static char gcBT_Buffer[APP_RX_STACK_SIZE];
static int snIdxBTBuffer = 0;
static uint32_t gps_coord_count = 0;
static bool sbGotCmdFromAndroid = false;
/**
 *
{37.336229, -121.881941},
{37.335896, -121.881747},
{37.335441, -121.881394},
{37.335125, -121.881168},
{37.334732, -121.880894}

Lshaped
{37.337284, -121.879799},
{37.337133, -121.880089},
{37.336871, -121.880727},
{37.336733, -121.880948},
{37.336503, -121.881495},
{37.336281, -121.881920},
{37.336229, -121.881941},
{37.335896, -121.881747},
{37.335441, -121.881394},
{37.335125, -121.881168},
{37.334732, -121.880894}
 */
static tGPSLatLng shardcodedGPSpath_0[] = {
                {37.336229, -121.881941},
                {37.335896, -121.881747},
                {37.335441, -121.881394},
                {37.335125, -121.881168},
                {37.334732, -121.880894}
            };
static tGPSLatLng shardcodedGPSpath_1[] = {
                {37.337284, -121.879799},
                {37.337133, -121.880089},
                {37.336871, -121.880727},
                {37.336733, -121.880948},
                {37.336503, -121.881495},
                {37.336281, -121.881920},
                {37.336229, -121.881941},
                {37.335896, -121.881747},
                {37.335441, -121.881394},
                {37.335125, -121.881168},
                {37.334732, -121.880894}
            };
static tGPSLatLng* spHardCodedGPSPaths[] = {shardcodedGPSpath_0, shardcodedGPSpath_1};

#define test_gps_dummy 0

can_t canx = can2;
bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    can_msg_t can_msg = { 0 };
    can_msg.msg_id                = mid;
    can_msg.frame_fields.data_len = dlc;
    memcpy(can_msg.data.bytes, bytes, dlc);

    return CAN_tx(canx, &can_msg, 0);

}

extern "C"
{
#if 0
void rx_msg(void)
{
    can_msg_t rx_msg = {0};
    SENSOR_LIDAR_OBSTACLE_INFO_t LidarObstacleMsg;
    while(CAN_rx(canx,&rx_msg,0))
    {
        dbc_decode_SENSOR_LIDAR_OBSTACLE_INFO(&LidarObstacleMsg,
                rx_msg.data.bytes,
                &SENSOR_LIDAR_OBSTACLE_INFO_HDR);
        switch(rx_msg.msg_id)
        {
            case MOTOR_HEARTBEAT_HDR.mid:
            mheartbeat_status |= (0x01 << MOTOR_LED);
            break;
            defautlt:
            break;
        }
    }

    // send data to android application.

}
#endif
size_t bt_init()
{
    Uart3& u3 = Uart3::getInstance();
    // Initially baudrate set it 9600 with tx and rx fifo queue size set it to 960

    u3.init(9600, 1000, 960);
    tBridgeCtrlrCtx* pBridgeCtrlrCtx = (tBridgeCtrlrCtx*)calloc(sizeof(tBridgeCtrlrCtx), 1);
    return (size_t)pBridgeCtrlrCtx;
}


void bt_ops_100Hz()
{
    uint32_t i,j;
    char ip = 0;

    // send data first
    Uart3& u3 = Uart3::getInstance();

    // if you want to send the data from lpc17xx to android app
    // or send gps location it received from GEO module to the android app

    while(u3.getChar(&ip, 0))
    {
        gcBT_Buffer[snIdxBTBuffer++] = ip;
        if(snIdxBTBuffer == APP_RX_STACK_SIZE)
        {
            gcBT_Buffer[0] = '\0';
            snIdxBTBuffer = 0;
        }
        else if(ip == '\n')
        {
            sbGotCmdFromAndroid = true;
            gcBT_Buffer[snIdxBTBuffer++] = '\0';
        }

    }

    if(sbGotCmdFromAndroid)
    {
        sbGotCmdFromAndroid = false;
        snIdxBTBuffer = 0;
        if(gcBT_Buffer[0] != '\0')
        {
               //don't use printf()
               LOGV("%s\n",gcBT_Buffer);

               // if message is TRON send turn of motor command to motor
               if(!strncmp(gcBT_Buffer, "TRON", 4))
                   turn_on_motor_request();

               else if(!strncmp(gcBT_Buffer, "TROFF", 5)) {
                   turn_off_motor_request();
               }
               else{

                   // get the first gps_coords_count which will be in this format xx;
                   char *chckpt_num = strtok(gcBT_Buffer,";");
                   // add condition to check this is gps checkpoint only, later
                   if(chckpt_num)
                   {
                       //u0_dbg_printf("[%s]\n", chckpt_num);
                       sscanf(chckpt_num,"%lu",&gps_coord_count);
                       if(gps_coord_count < 256)
                       {
                           for(i = 0; i < gps_coord_count; i++) {
                               token[i] = strtok(NULL, ";");
                           }

                           //u0_dbg_printf("i=%d c_count=%d\n", i, gps_coord_count);
                           if(i == gps_coord_count)
                           {
                               send_gps_data_count(gps_coord_count);
                               for(j = 0; j < gps_coord_count; j++){

                                   LD.setNumber(j);
                                   chckptlist[2*j] = strtok(token[j], ",");
                                   chckptlist[2*j+1] = strtok(NULL, ",");
                                   //u0_dbg_printf("L=%s Lo=%s\n", chckptlist[2*j], chckptlist[2*j+1]);
                                   send_gps_data((double)(atof(chckptlist[2*j])), (double)(atof(chckptlist[2*j+1])));
                               }
                           }
                       }
                   }
               }
               gcBT_Buffer[0] = '\0';
           }


    }

}

GPIO *led_pin;
void can_init(void)
{
    //check the tx and rx queue size too.
    //bool ret  = CAN_init(canx, 100, 10, 1, NULL, NULL);
    CAN_init(canx, 100, 10, 100, NULL, NULL);
    //printf("initialized return %d\n", ret);
    // fullCAN filter can be added with range of can msg id
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(canx);
}
#if 1
void send_heartbeat_to_master(void)
{
    // when health is good send 0x1 otherwise send 0x0
    BLE_HEARTBEAT_t msg = {0x1};

    dbc_encode_and_send_BLE_HEARTBEAT(&msg);

}
#endif
void turn_off_motor_request(void)
{
    // when health is good send 0x1 otherwise send 0x0
    BLE_START_STOP_CMD_t msg = {0};
    msg.BLE_START_STOP_CMD_start = 0x0;
    dbc_encode_and_send_BLE_START_STOP_CMD(&msg);

}

void turn_on_motor_request(void)
{
    // when health is good send 0x1 otherwise send 0x0
    BLE_START_STOP_CMD_t msg = {0};
    msg.BLE_START_STOP_CMD_start = 0x1;
    dbc_encode_and_send_BLE_START_STOP_CMD(&msg);

}

void send_gps_data_count(int count)
{
    BLE_GPS_DATA_CNT_t d1_cnt={0};
    d1_cnt.BLE_GPS_COUNT = count;
  //  printf("\nGPS Coord Count:%d", d1_cnt.BLE_GPS_COUNT);
    //LD.setNumber((uint8_t)count);
    //u0_dbg_printf(":%d\n",count);
    if(d1_cnt.BLE_GPS_COUNT > 0)
    {
        dbc_encode_and_send_BLE_GPS_DATA_CNT(&d1_cnt);
    }



}

void send_gps_data(double lat, double longs)
//void send_gps_data(float lat, float longs, int count)    //Enable this Function call when doing Testing using "test_gps_dummy" without android
{
    BLE_GPS_DATA_t d1 = {0};
    d1.BLE_GPS_long = (float)longs;
    d1.BLE_GPS_lat = (float)lat;

#if test_gps_dummy
    //Enable below when testing without Android
    BLE_GPS_DATA_CNT_t d1_cnt={0};
    d1_cnt.BLE_GPS_COUNT = count;
    printf("\nGPS Coord Count:%d", d1_cnt.BLE_GPS_COUNT);
    dbc_encode_and_send_BLE_GPS_DATA_CNT(&d1_cnt);
#endif

    //u0_dbg_printf("\nGPS Lat:%lf, Long:%lf from Android",d1.BLE_GPS_lat, d1.BLE_GPS_long);
    //dbc_encode_and_send_BLE_GPS_INT(&d1);
    dbc_encode_and_send_BLE_GPS_DATA(&d1);
}


void vHandleCANIncoming(size_t ahBrigdeCtrlrCtx)
{
    tBridgeCtrlrCtx* pBridgeCtrlrCtx = (tBridgeCtrlrCtx*)ahBrigdeCtrlrCtx;
    can_msg_t rx_msg = {0};

    while(CAN_rx(canx,&rx_msg,0))
    {
        switch(rx_msg.msg_id)
        {
            case GEO_TELECOMPASS_HDR.mid:
            {
                GEO_TELECOMPASS_t comp = {0};
                dbc_decode_GEO_TELECOMPASS(&comp,
                        rx_msg.data.bytes,
                        &GEO_TELECOMPASS_HDR);
                /** update GPS info in the DEBUG cJSON */
                /** 1. heading */
                pBridgeCtrlrCtx->fCompassHeading = comp.GEO_TELECOMPASS_compass;
                /** 2. bearing */
                pBridgeCtrlrCtx->fCompassBearing = comp.GEO_TELECOMPASS_bearing_angle;
                /** 3. dist_to_dest */
                pBridgeCtrlrCtx->fDistToDest = comp.GEO_TELECOMPASS_distance;
                /** 4. dest_reach_stat */
                pBridgeCtrlrCtx->bDestReachStat = comp.GEO_TELECOMPASS_destination_reached;
                /** 5. Checkpoint ID */
                pBridgeCtrlrCtx->nCheckpointID = comp.GEO_TELECOMPASS_checkpoint_id;
            }
            break;
            case GEO_TURNING_ANGLE_HDR.mid:
            {
                GEO_TURNING_ANGLE_t geoTurningAngle = {0};
                dbc_decode_GEO_TURNING_ANGLE(&geoTurningAngle, rx_msg.data.bytes,
                    &GEO_TURNING_ANGLE_HDR);
                pBridgeCtrlrCtx->nTurningAngle = geoTurningAngle.GEO_TURNING_ANGLE_degree;
            }
            break;
            case GEO_TELEMETRY_LOCK_HDR.mid:
            {
                GEO_TELEMETRY_LOCK_t geoLock = {0};
                dbc_decode_GEO_TELEMETRY_LOCK(&geoLock, rx_msg.data.bytes, &GEO_TELEMETRY_LOCK_HDR);
                pBridgeCtrlrCtx->bGpsLock = geoLock.GEO_TELEMETRY_lock;
            }
            break;
            case SENSOR_LIDAR_OBSTACLE_INFO_HDR.mid:
            {
                SENSOR_LIDAR_OBSTACLE_INFO_t lidar_info = {0};
                dbc_decode_SENSOR_LIDAR_OBSTACLE_INFO(&lidar_info, rx_msg.data.bytes, &SENSOR_LIDAR_OBSTACLE_INFO_HDR);
                memcpy((void*)&pBridgeCtrlrCtx->lidar_obstacle, (void*)&lidar_info,sizeof(SENSOR_LIDAR_OBSTACLE_INFO_t));
            }
            break;

            case GEO_CURRENT_COORD_HDR.mid:
            {
                GEO_CURRENT_COORD_t geo_curr_coord={0};
                dbc_decode_GEO_CURRENT_COORD(&geo_curr_coord, rx_msg.data.bytes,&GEO_CURRENT_COORD_HDR);
                pBridgeCtrlrCtx->fcurrLat = geo_curr_coord.GEO_CURRENT_COORD_LAT;
                pBridgeCtrlrCtx->fcurrLong = geo_curr_coord.GEO_CURRENT_COORD_LONG;
            }
        }
    }
    // send this code to android
}

#define MAX_DEBUG_STR_LEN (1024 * 4)
static char spcDebugString[MAX_DEBUG_STR_LEN];

void vSendDebugInfoToAndroid(size_t ahBrigdeCtrlrCtx)
{
    tBridgeCtrlrCtx* pBridgeCtrlrCtx = (tBridgeCtrlrCtx*)ahBrigdeCtrlrCtx;
    int32_t nLen, i;
    i = 0;
    Uart3& u3 = Uart3::getInstance();
    nLen = snprintf((char*)spcDebugString, sizeof(spcDebugString), spcDEBUG_JSON_FORMAT,
            pBridgeCtrlrCtx->bGpsLock,
            pBridgeCtrlrCtx->fCompassHeading,
            pBridgeCtrlrCtx->fCompassBearing,
            pBridgeCtrlrCtx->nTurningAngle,
            pBridgeCtrlrCtx->fDistToDest,
            pBridgeCtrlrCtx->bDestReachStat,
            pBridgeCtrlrCtx->nCheckpointID,
            pBridgeCtrlrCtx->lidar_obstacle.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR0,
            pBridgeCtrlrCtx->lidar_obstacle.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR1,
            pBridgeCtrlrCtx->lidar_obstacle.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR2,
            pBridgeCtrlrCtx->lidar_obstacle.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR3,
            pBridgeCtrlrCtx->lidar_obstacle.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR4,
            pBridgeCtrlrCtx->lidar_obstacle.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR5,
            pBridgeCtrlrCtx->lidar_obstacle.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR6,
            pBridgeCtrlrCtx->lidar_obstacle.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR7,
            pBridgeCtrlrCtx->lidar_obstacle.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR8,
            pBridgeCtrlrCtx->lidar_obstacle.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR9,
            pBridgeCtrlrCtx->lidar_obstacle.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR10,
            pBridgeCtrlrCtx->lidar_obstacle.SENSOR_LIDAR_OBSTACLE_INFO_SECTOR11,
            gps_coord_count,
            pBridgeCtrlrCtx->fcurrLat,
            pBridgeCtrlrCtx->fcurrLong
            )
            + 1 /** +1 for the NULL character */
            ;
    spcDebugString[nLen-1] = '\0';
    LOGD("snd %d %d [%s]\n", (int)nLen, (int)strlen(spcDEBUG_JSON_FORMAT), spcDebugString);
    while(i < nLen) {
        LOGD("%c", spcDebugString[i]);
        if(!u3.putChar(spcDebugString[i], 0)) {
            return;
        }
        i++;
    }
    LOGD("!\n");
}

void vSendHardcodedGPSCheckpoints(size_t ahBrigdeCtrlrCtx, int anPathIdx)
{
    /** send count */
    BLE_GPS_DATA_CNT_t dataCnt = {0};
    if(anPathIdx == 0)
        dataCnt.BLE_GPS_COUNT = sizeof(shardcodedGPSpath_0) / sizeof(tGPSLatLng);
    else if(anPathIdx == 1)
        dataCnt.BLE_GPS_COUNT = sizeof(shardcodedGPSpath_1) / sizeof(tGPSLatLng);

    printf("count=%d\n", dataCnt.BLE_GPS_COUNT);
    dbc_encode_and_send_BLE_GPS_DATA_CNT(&dataCnt);

    /** GPS checkpoints */
    for(int i = 0; i < dataCnt.BLE_GPS_COUNT; i++)
    {
        BLE_GPS_DATA_t gpsData = {0};
        gpsData.BLE_GPS_lat = spHardCodedGPSPaths[anPathIdx][i].fLat;
        gpsData.BLE_GPS_long = spHardCodedGPSPaths[anPathIdx][i].fLong;
        printf("GPS %f %f\n", gpsData.BLE_GPS_lat, gpsData.BLE_GPS_long);
        dbc_encode_and_send_BLE_GPS_DATA(&gpsData);
    }
}

void get_gps_locks(void)
{

    GEO_TELEMETRY_LOCK_t geo_lock = {0};
    can_msg_t rx_msg = {0};
    while(CAN_rx(canx,&rx_msg,0))
    {
        dbc_decode_GEO_TELEMETRY_LOCK(&geo_lock,
                rx_msg.data.bytes,
                &GEO_TELEMETRY_LOCK_HDR);

       }
}
}
