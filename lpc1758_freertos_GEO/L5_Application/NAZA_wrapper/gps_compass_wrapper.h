/*
 * gps_compass_wrapper.h
 *
 *  Created on: Oct 27, 2017
 *      Author: sneha
 */

#ifndef GPS_COMPASS_WRAPPER_H_
#define GPS_COMPASS_WRAPPER_H_

typedef struct{
    float lat_val;
    float long_val;
    int nIdx;
}checkpoint_t;

void GpsInit(void);
int getGpsCompassValue(void);
void getCheckpoints(checkpoint_t* apcheckpoint);
void GpsUartInit(void);
void GpsCanInit(void);
void CANbusResetwhenOff(void);
int CANMsgRx(uint32_t acount);
void GpsTransaction100Hz(void);
void GpsTransaction1Hz(void);
void reportNavStatus();
void sendHeartbeat(void);
void sendDistancetoDisplay(void);
void checkGpslock(void);
void getBearingAngle(checkpoint_t* apcurr_checkpoint);
void getTurningAngle(void);
void checkDestinationReached(bool isNearestCpCalculationRequired);
double calculateDistance(double sourceLatitudeL, double sourceLongitudeL, double destinationLatitudeL, double destinationLongitudeL);
void GpsledlockInit(void);
void gpsDebug(void);
void getDistanceRemaining(checkpoint_t* apcurr_checkpoint);
bool sdCardRead(void);
void sendCanturningAngle(double turningAngleL);
void sendCanLatLong(double longitudeL,double latitudeL);
#endif /* GPS_COMPASS_WRAPPER_H_ */
