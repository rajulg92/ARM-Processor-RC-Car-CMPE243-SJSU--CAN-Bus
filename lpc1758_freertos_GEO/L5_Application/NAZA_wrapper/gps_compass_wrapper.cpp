#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include "gps_compass_wrapper.h"
#include "_can_dbc/generated_can.h"
#include "NAZA_GPS/NazaDecoder.hpp"
#include "periodic_scheduler/periodic_callback.h"
#include "can.h"
#include "Uart3.hpp"
#include "printf_lib.h"
#include "math.h"
#include "periodic_scheduler/error_stat_display.hpp"
#include "io.hpp"
#include "gpio.hpp"
#include "command_handler.hpp"
#include "storage.hpp"
#include "task.h"
#define SINGLE_CHECKPOINT 1
#define UART_QUEUE_SIZE (12*3+64+18)

#define TO_NEAREST
//static int nextCheckpointIndex=0;uncomment this when SINGLE_CHECKPOINT 0
//bet eng building and clark hall
//double mockLatitude = 37.336353, mockLongitude = -121.882016;
//wells farg0
double mockLatitude=37.337328,mockLongitude=-121.879733;

//double mockLatitude = 37.337264, mockLongitude = -121.879934;
#define PI  3.1432
#define PI_DEGREES 180

static double compass = 0;
static double longitude,latitude;
static double bearingAngle;
static char recieve;
static double turningAngle;
double distanceRemaining;
int displayLD;

#define MAX_CHECKPOINTS (1024)
static checkpoint_t* spCheckpoints=NULL;
static checkpoint_t* spCheckpointsTmp=NULL;
static checkpoint_t* spCurrCheckpoints=NULL;
static uint8_t sncheckpt_count=0;
static uint8_t sncheckpt_rxd=0;

//sstatic double checkpointLon[] ={};
int nCharactersReadFromNAZA = 0;
#define DEBUG_BLOCK_SZ (1024 * 6)
static char pcDebugBlock[DEBUG_BLOCK_SZ];
static int gnDebugBlockUsage = 0;
bool gbWriteDebugInfo = false;

static checkpoint_t* sGetNearestCheckpoint(double* apDistanceToNearest);
static double sGetDistanceToDest(checkpoint_t* apFrom);

const can_std_id_t slist[]      = {CAN_gen_sid(can2,0xD4), CAN_gen_sid(can2, 0xD5)};

//not thread safe
static void sDebugGPS(double afLatittude, double afLongitude, double afTurning,double afBearing,double afCompass)
{
    char pcLocal[100] = {0};
    int nDebugBlockUsage = gnDebugBlockUsage + snprintf(pcLocal, 100, "GPS %lu %f %f %f %f %f\n", xTaskGetTickCount(), afLatittude,
            afLongitude, afTurning,afBearing,afCompass);
    if(nDebugBlockUsage <= DEBUG_BLOCK_SZ)
    {
        strcat(pcDebugBlock, pcLocal);
        gnDebugBlockUsage = nDebugBlockUsage;
    }
    else
    {
        gbWriteDebugInfo = true;
    }
}

//not thread safe
#if 0
static void sDebugCompass(double afCompass)
{
    char pcLocal[100] = {0};
    int nDebugBlockUsage = gnDebugBlockUsage + snprintf(pcLocal, 100, "COM %lu %f\n", xTaskGetTickCount(), afCompass);
    if(nDebugBlockUsage <= DEBUG_BLOCK_SZ)
    {
        strcat(pcDebugBlock, pcLocal);
        gnDebugBlockUsage = nDebugBlockUsage;
    }
    else
    {
        gbWriteDebugInfo = true;
    }
}
#endif

//static double gbiasCompass;
//static double gDistance;
static uint8_t glockedStatus;
//static double correctAngle=360;
//37.337264 -121.87993437.337264 -121.879934
bool checkpointReached;
bool destinationReachedFlag;
BLE_GPS_DATA_CNT_t gpsCount;
//static int checkpoint_index=0;
can_msg_t msg = {0};
//static double bias;
uint8_t destinationReached;
double degreeToRadian (double degree) { return (degree * PI / 180); };
double radianToDegree (double radian) { return (radian * 180 / PI); };
extern const uint32_t          BLE_GPS_DATA__MIA_MS=3000;
extern const BLE_GPS_DATA_t    BLE_GPS_DATA__MIA_MSG={0};

NazaDecoder naza = NazaDecoder();
GPIO pin0_1(P0_1);

bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    can_msg_t can_msg = { 0 };
    can_msg.msg_id                = mid;
    can_msg.frame_fields.data_len = dlc;
    memcpy(can_msg.data.bytes, bytes, dlc);
    return CAN_tx(can2, &can_msg, 0);
}

void GpsCanInit(void)
{
    CAN_init(can2,100,100,32,NULL,NULL);
    //CAN_fullcan_add_entry(can2 ,CAN_gen_sid(can2,0xD4),CAN_gen_sid(can2,0xFFFF));
   // CAN_setup_filter(slist,2,NULL,0,NULL,0,NULL,0);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(can2);
}

void GpsUartInit(void)
{
    /* Why rxQ is 94B ?;
     * The max data including the GPS string, compass string,
     * version string - is 98B:
     * Compass string: 12B
     * GPS String: 64B
     * Version: 18B
     * Total: 12 + 64 + 18 = 94B
     * Also delay between two: 
     * GPS values = 240 - 250ms (tested with SD dump; timestamps)
     * Compass values = 30ms (exact; tested with SD dump; timestamps)
     * Version ID; 2s (not tested; not used in code as well)
     * At any given time, we can expect all over the UART (worst case)
     * because of which we allocate 94B here
     * IMPORTANT NOTE:
     * We observed stale data in NAZA: receive + parse code
     * - This had to do with our design where we call NazaDecoder.decode()
     * from GpsTransaction100Hz(), getCompass() [TODO: work on func naming]
     * TODO: WE SHALL CHANGE THE DESIGN
     * Half parsed GPS/compass data in NazaDecoder.cpp will affect our logic
     * - we dont have protection over reading half-parsed info from NazaDecoder
     * TEMPORARY_FIX* was made for the last release (which is debatable as of now)
     */
    Uart3::getInstance().init(115200,UART_QUEUE_SIZE,1);
}

void GpsledlockInit(void)
{
    pin0_1.setAsOutput();
   // pin0_1.enablePullDown();
}

void GpsInit(void)
{
    GpsCanInit();
    GpsUartInit();
    GpsledlockInit();
}

void CANbusResetwhenOff(void)
{
    if(CAN_is_bus_off(can2))
    {
        ErrorStatDisplay::displayErrorStat(kErrorStatDisplay_BusOff);
        CAN_reset_bus(can2);
    }
}

double calculateBearingAngle(double sourceLatitude, double sourceLongitude, double destinationLatitude, double destinationLongitude)
{

#if 0
    double longitudeDiff = (destinationLongitude - sourceLongitude);

    double y = sin(longitudeDiff) * cos(destinationLatitude);
    double x = cos(sourceLatitude) * sin(destinationLatitude) - sin(sourceLatitude) * cos(destinationLatitude) * cos(longitudeDiff);

    double bearingAngle = atan2(y, x);

    bearingAngle = radianToDegree(bearingAngle);
    bearingAngle = fmod((bearingAngle + 360),360);
    bearingAngle = 360 - bearingAngle; // count degrees counter-clockwise - remove to make clockwise

    return bearingAngle;
#endif
#if 0
    double a = degreeToRadian(sourceLatitude);
    double b = degreeToRadian(sourceLongitude);
    double c = degreeToRadian(destinationLatitude);
    double d = degreeToRadian(destinationLongitude);

    if (cos(c) *sin(d - b) == 0)
        if (c > a)
            return 0;
        else
            return 180;
    else
    {
        double angle = atan2(cos(c) * sin(d - b), sin(c) *cos(a) - sin(a) * cos(c) * cos(d - b));
        angle = fmod((angle * 180 / PI + 360) , 360);
        return angle;

    }

#endif
    double lat1=sourceLatitude;
    double lon1=sourceLongitude;
    double lat2=destinationLatitude;
    double lon2=destinationLongitude;
    double head = 0 , lon_difference = 0;
    lat1 = (lat1 * PI)/ PI_DEGREES;
    lon1 = (lon1 * PI)/ PI_DEGREES;
    lat2 = (lat2 * PI)/ PI_DEGREES;
    lon2 = (lon2 * PI)/ PI_DEGREES;
    lon_difference = lon2 - lon1;
    head = atan2((sin(lon_difference)*cos(lat2)), ((cos(lat1)*sin(lat2))-(sin(lat1)*cos(lat2)*cos(lon_difference)))) ;
    head = (head * PI_DEGREES)/ PI;
    if( head <= 0 )
        head += 360;
    return head;

}

/*to send turning flag and compass through CAN*/
void sendCanturningFlagCompass(bool checkpointReachedL, bool destinationReachedL,double compassL,
            double bearingAngleL,double distanceRemainingL,int8_t sncurr_checkpt_idxL)
{
    GEO_TELECOMPASS_t    checkpointFlagCompass  = {0};
    checkpointFlagCompass.GEO_TELECOMPASS_destination_reached=destinationReachedL;
    checkpointFlagCompass.GEO_TELECOMPASS_bearing_angle=bearingAngleL;
    checkpointFlagCompass.GEO_TELECOMPASS_compass=compassL;
    checkpointFlagCompass.GEO_TELECOMPASS_distance=distanceRemainingL;
    checkpointFlagCompass.GEO_TELECOMPASS_checkpoint_id=sncurr_checkpt_idxL;
    dbc_encode_and_send_GEO_TELECOMPASS(&checkpointFlagCompass);
}

/*to send lattitude and longitude through CAN*/
void sendCanLatLong(double longitudeL,double latitudeL)
{
    GEO_CURRENT_COORD_t   coordinates ;

    if (naza.isLocked() == 1)
    {
        coordinates.GEO_CURRENT_COORD_LONG            = longitudeL;
        coordinates.GEO_CURRENT_COORD_LAT             = latitudeL;
        dbc_encode_and_send_GEO_CURRENT_COORD(&coordinates);
    }
    else
    {
        //coordinates.GEO_CURRENT_COORD_LONG            = -121.881362;
       // coordinates.GEO_CURRENT_COORD_LAT             =  37.335246;
    }

}

/*function to send turningAngle*/
void sendCanturningAngle(double turningAngleL)
{
    GEO_TURNING_ANGLE_t  turningAngleCan = {0};
    turningAngleCan.GEO_TURNING_ANGLE_degree  = turningAngleL;
    dbc_encode_and_send_GEO_TURNING_ANGLE(&turningAngleCan);
}

/*function to send heartbeat through Can*/
void sendHeartbeat(void)
{
    GEO_HEARTBEAT_t heartbeat_local;
    heartbeat_local.GEO_HEARTBEAT_signal=1;
    dbc_encode_and_send_GEO_HEARTBEAT(&heartbeat_local);
}

/*Function to receive CAN message from BLE for the checkpoints*/
int CANMsgRx(uint32_t count)
{
    can_msg_t can_msg = { 0 };
    while(CAN_rx(can2,&can_msg,0))
    {
        switch(can_msg.msg_id)
        {
            case BLE_GPS_DATA_CNT_HDR.mid :
            {
                BLE_GPS_DATA_CNT_t ble_gps_checkpt_cnt= {0};
                dbc_decode_BLE_GPS_DATA_CNT(&ble_gps_checkpt_cnt,can_msg.data.bytes,&BLE_GPS_DATA_CNT_HDR);
                //ToDo Add condition to check checkpoint count

                if(ble_gps_checkpt_cnt.BLE_GPS_COUNT > 0)
                {
                    destinationReachedFlag=0;
                    LE.set(4,false);
                    sncheckpt_count=0;
                    sncheckpt_rxd = 0;
                    sncheckpt_count = ble_gps_checkpt_cnt.BLE_GPS_COUNT;
                    LD.setNumber((char)sncheckpt_count);
                    //u0_dbg_printf("%d\n", sncheckpt_count);

                    if(!spCheckpoints)
                    {
                        spCheckpoints = (checkpoint_t*)calloc(sizeof(checkpoint_t), MAX_CHECKPOINTS);
                    }
                    spCurrCheckpoints = NULL;
                    spCheckpointsTmp = spCheckpoints;

                }

            }
            break;

            case BLE_GPS_DATA_HDR.mid:
            {
                BLE_GPS_DATA_t ble_gps_checkpt_data = { 0 };
                dbc_decode_BLE_GPS_DATA(&ble_gps_checkpt_data, can_msg.data.bytes, &BLE_GPS_DATA_HDR);


                if(spCheckpointsTmp && sncheckpt_count)
                {
                    spCheckpointsTmp->nIdx = sncheckpt_rxd;
                    sncheckpt_rxd++;
                    spCheckpointsTmp->lat_val = ble_gps_checkpt_data.BLE_GPS_lat;
                    spCheckpointsTmp->long_val = ble_gps_checkpt_data.BLE_GPS_long;
                    //u0_dbg_printf("1:%f %f\n",spCheckpointsTmp->lat_val,spCheckpointsTmp->long_val);
                    if(sncheckpt_rxd < sncheckpt_count)
                    {
                        spCheckpointsTmp++;
                    }
                    else
                    {
                        //u0_dbg_printf("recived %d\n",sncheckpt_rxd);
                        spCurrCheckpoints = spCheckpoints;
                    }
                }
            }
            break;

            default:
                break;
        }
    }

    return 1;
}

void getCheckpoints(checkpoint_t* apcheckpoint)
{
#if 0
    can_msg_t can_msg = { 0 };''
    BLE_GPS_DATA_t rx_cmd={0};
    while(CAN_rx(can2,&can_msg,0))
    {
        dbc_msg_hdr_t can_msg_hdr;
        can_msg_hdr.dlc=can_msg.frame_fields.data_len;
        can_msg_hdr.mid=can_msg.msg_id;
        dbc_decode_BLE_GPS_DATA(&rx_cmd, can_msg.data.bytes, &can_msg_hdr);
        dbc_decode_BLE_GPS_DATA_CNT(&gpsCount, can_msg.data.bytes, &can_msg_hdr);;
        //checkpointLat[checkpoint_index]=rx_cmd.BLE_GPS_lat;
        //scheckpointLon[checkpoint_index]=rx_cmd.BLE_GPS_long;
        //checkpoint_index++;
        ErrorStatDisplay::clearErrorStat(kErrorStatDisplay_SSCmdMIAOccured);
    }

    if (dbc_handle_mia_BLE_GPS_DATA(&rx_cmd,10))
    {
        ErrorStatDisplay::displayErrorStat(kErrorStatDisplay_SSCmdMIAOccured);
        memset(&rx_cmd, 0, sizeof(rx_cmd));
    }

#endif
}

void reportNavStatus()
{


    sendCanturningFlagCompass(checkpointReached,destinationReachedFlag,compass,
            bearingAngle,distanceRemaining,(spCurrCheckpoints ? spCurrCheckpoints->nIdx+1 : 0));
    sendCanturningAngle(turningAngle);
    sendCanLatLong(longitude,latitude);

#ifdef DUMP_GPS
sDebugGPS(latitude, longitude, turningAngle,compass,bearingAngle);
#endif
}

int getGpsCompassValue(void)
{
    int nCountGEO = 0;
    int nCountCompass = 0;

    while(Uart3::getInstance().getChar(&recieve,0))
    {

        int input=recieve;
        uint8_t decodedMessage = naza.decode(input);
        switch (decodedMessage)
        {

            case NazaDecoder::NAZA_MESSAGE_GPS_TYPE:
            {
                nCountGEO++;
                longitude     = naza.getLongitude();
                latitude      = naza.getLatitude();

                /*Every 250 ms this processing is done*/
                if(spCurrCheckpoints)
                {
                    getBearingAngle(spCurrCheckpoints);
                    getTurningAngle();
                    getDistanceRemaining(spCurrCheckpoints);
                    checkDestinationReached(true);
                }

                reportNavStatus();
                //xQueueReset(Uart3::getInstance().getRxQueue());
                return nCountCompass;
                /**< return here to avoid reading bits of compass/next 
                 * GPS within this iteration
                 * TEMPORARY_FIX; Read comments in UART_init above */

                break;

            }
            case NazaDecoder::NAZA_MESSAGE_MAGNETOMETER_TYPE:
            {
                nCountCompass++;
                if(nCountCompass > 3)
                {
                    xQueueReset(Uart3::getInstance().getRxQueue());
                    return nCountCompass;
                }
                /*Every 30 ms compass value is coming*/
                compass = naza.getHeading();
                //return nCountCompass;
                /**< return here to avoid reading bits of compass/next 
                 * GPS within this iteration
                 * TEMPORARY_FIX; Read comments in UART_init above*/
                break;
            }

            default:
                break;
        }

        //LD.setNumber(nCountCompass * 10 + nCountGEO);

#if 0
        if(nCountCompass > 3 || nCountGEO > 1)
        {
            xQueueReset(Uart3::getInstance().getRxQueue());
            break;
        }
#endif
    }

    return nCountCompass;
}

/*to calculate distance*/
double calculateDistance(double sourceLatitudeL, double sourceLongitudeL, double destinationLatitudeL, double destinationLongitudeL)
{
    double theta, distance;
    theta = sourceLongitudeL - destinationLongitudeL;
    distance = sin(degreeToRadian(sourceLatitudeL)) * sin(degreeToRadian(destinationLatitudeL)) + cos(degreeToRadian(sourceLatitudeL)) * cos(degreeToRadian(destinationLatitudeL)) * cos(degreeToRadian(theta));
    distance = acos(distance);
    distance = radianToDegree(distance);
    distance = distance * (double)60 * (double)1.1515;
    distance = distance * (double)1609.344;
    return distance;
}

/*Function for Gps lock*/
void checkGpslock(void)
{
    glockedStatus = naza.isLocked();
    if (glockedStatus==1)
    {
        pin0_1.setHigh();
    }
    else
    {
        pin0_1.setLow();
    }

}

/*Debug function to check lattitude longitude and distance remaining*/
void gpsDebug(void)
{
    //u0_dbg_printf("1:%f\n",latitude);
    //u0_dbg_printf("2:%f\n",longitude);
   //u0_dbg_printf("1:%d\n",nCharactersReadFromNAZA);
    // u0_dbg_printf("c:%f\n",compass);
    //u0_dbg_printf("b%f\n",bearingAngle);
}

/*send Gps lock*/
void sendGpslock(void)
{
    GEO_TELEMETRY_LOCK_t gps_locksend;
    gps_locksend.GEO_TELEMETRY_lock=glockedStatus;
   // u0_dbg_printf("l:%u\n",glockedStatus);
    dbc_encode_and_send_GEO_TELEMETRY_LOCK(&gps_locksend);

}

/*calculate distance remaining*/
void getDistanceRemaining(checkpoint_t* apcurr_checkpoint)
{
    #if (SINGLE_CHECKPOINT == 1)
    distanceRemaining=calculateDistance(latitude,longitude,apcurr_checkpoint->lat_val,apcurr_checkpoint->long_val);
    #endif
}

/*Send the quadrant of turning angle on LCD display*/
void sendDistancetoDisplay(void)
{
    LD.setNumber(distanceRemaining);
}

/*Function to calculate turning Angle based on  heading and compass */
void getTurningAngle(void)
{
    turningAngle = bearingAngle-compass;

    if (turningAngle>180)
        turningAngle=turningAngle-360;

    if (turningAngle<-180)
        turningAngle=turningAngle+360;
}

/*function to calculate bearing angle between 2 lattitude and longitude points*/
void getBearingAngle(checkpoint_t* apcurr_checkpoint)
{
    if(apcurr_checkpoint)
    {
        bearingAngle = calculateBearingAngle(latitude, longitude, apcurr_checkpoint->lat_val, apcurr_checkpoint->long_val);
    }
}

/*A function to store values which we are getting into SD card*/
bool sdCardRead(void)
{
    static int nOnce = 0;
    const char* pFilename = "1:Gpslogs.txt";
#if 1
    if(!nOnce)
    {
        char pcMock;
        Storage::write(pFilename,&pcMock, 0);
        nOnce = 1;
    }
#endif
  //  u0_dbg_printf("1:%s",gpsTempData);
    FRESULT sdDATA = FR_OK;
    if(gbWriteDebugInfo)
    {
        sdDATA = Storage::append(pFilename, pcDebugBlock, gnDebugBlockUsage);
        pcDebugBlock[0] = '\0';
        gnDebugBlockUsage = 0;
        gbWriteDebugInfo = false;
    }

    if(sdDATA>0)
    {
      return true;
    }
    else
    {
      return false;

    }
//    index++;
}


/*A function to check if destination has reached or not*/
/*Set a flag based on the turning angle                */

void checkDestinationReached(bool isNearestCpCalculationRequired)
{
    /* If checkpoint reached, Increment current checkpoint pointer to next checkpoint, also increment index*/
    if (distanceRemaining < 2.5)
    {
        checkpointReached=1;

        if(spCurrCheckpoints->nIdx == (sncheckpt_count-1))
        {
            destinationReachedFlag=1;
            sncheckpt_count = 0;
            spCurrCheckpoints = NULL;
            LE.set(4,true);
            return;
        }

        spCurrCheckpoints++;
    }
    else
    {
        checkpointReached=0;
#ifdef TO_NEAREST
        if(isNearestCpCalculationRequired)
        {
            /** also, update the next checkpoint here
             * 1) Dist_from_nearest_cp_to_dest
             * 2) Dist_from_curr_cp_to_dest */
            double fDistToNearestCp = 0.0;
            checkpoint_t* pNearestCp = sGetNearestCheckpoint(&fDistToNearestCp);
            if((pNearestCp != spCurrCheckpoints)
                    && pNearestCp)
            {
                double fFromNearestToDest = fDistToNearestCp + sGetDistanceToDest(pNearestCp);
                double fFromCurrentToDest = distanceRemaining + sGetDistanceToDest(spCurrCheckpoints);
                if(fFromNearestToDest <= fFromCurrentToDest)
                {
                    spCurrCheckpoints = pNearestCp;
                }
            }
        }
#endif
    }

    return;
}

/*GPS transaction happening in 100Hz function*/
void GpsTransaction100Hz(void)
{


    /*Reads Compass and GPS values at 10ms interval and send the turning angle over CAN to MAster */
    getGpsCompassValue();

    /*Send the distance with respect to checkpoints on LD display*/


#if (SINGLE_CHECKPOINT ==0)
    getCheckpoints();
#endif

}

/*GPS transaction for Periodic 1Hz function*/
void GpsTransaction1Hz(void)
{
    /*Send Telemetry messages to BLE*/
    sendHeartbeat();
    sendGpslock();
    sdCardRead();

}


static checkpoint_t* sGetNearestCheckpoint(double* apDistanceToNearest)
{
    checkpoint_t* pNearestCp = NULL;
    checkpoint_t* pTmp = spCheckpoints;
    int i = 0;
    double fMin = 0.0;
    double fTmp = 0.0;

    if(sncheckpt_count <= 0)
    {
        return NULL;
    }

    pNearestCp = &pTmp[i];
    fMin = calculateDistance(latitude,longitude,pTmp[i].lat_val,pTmp[i].long_val);
    //find dist from car's current location to all the checkpoints
    for(i = 1; i < sncheckpt_count; i++)
    {
#if 1
        if(fMin > (fTmp = calculateDistance(latitude,longitude,pTmp[i].lat_val,pTmp[i].long_val)))
        {
            fMin = fTmp;
            pNearestCp = &pTmp[i];
        }
#endif
    }


    *apDistanceToNearest = fMin;
    return pNearestCp;
}

static double sGetDistanceToDest(checkpoint_t* apFrom)
{
    int i = 0;
    double fDist = 0.0;
    checkpoint_t* pTmp = spCheckpoints;

    if(!apFrom || apFrom->nIdx >= sncheckpt_count || !spCheckpoints)
        return fDist;

    for(i = apFrom->nIdx; i < sncheckpt_count-1; i++)
    {
        fDist += calculateDistance(pTmp[i].lat_val, pTmp[i].long_val, pTmp[i+1].lat_val, pTmp[i+1].long_val);
    }

    return fDist;
}
