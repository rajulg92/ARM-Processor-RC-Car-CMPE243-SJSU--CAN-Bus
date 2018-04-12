#include "Gps.hpp"
Gps::~Gps() {
}

void Gps::fillGpsInfo(GpsInfo* info) {
    info->latitude = getLatitude();
    info->longitude = getLongitude();
    info->altitude = getAltitude();
    info->speed = getSpeed();
    info->fixType = getFixType();
    info->satellites = getSatellites();
    info->courseOverGround = getCourseOverGround();
    info->verticalSpeedIndicator = getVerticalSpeedIndicator();
    info->horizontalDilutionOfPrecision = getHorizontalDilutionOfPrecision();
    info->verticalDilutionOfPrecision = getVerticalDilutionOfPrecision();
    info->year = getYear();
    info->month = getMonth();
    info->day = getDay();
    info->hour = getHour();
    info->minute = getMinute();
    info->second = getSecond();
    info->isLocked = isLocked();
}
