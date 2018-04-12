/****************************************************************************
 * geo_interface.cpp
 * @file :
 * @created : Oct 20,2017
 *
 ****************************************************************************/

#ifndef GEO_INTERFACE_HPP_
#define GEO_INTERFACE_HPP_

#include <stdint.h>

class IGeoController
{
    public:

    IGeoController();
    int16_t GetTurningAngle(void);
    void SetTurningAngle(uint16_t turnangle);

    GEO_TURNING_ANGLE_t GeoManeuveringMsg;
    GEO_TELECOMPASS_t GeoTeleCompassMsg;

    //GEO_HEARTBEAT_t GeoHeartbeatMsg;

    private:
    int16_t mturning_angle;
    int16_t mdistance_to_destination; //in meters


};



#endif /* GEO_INTERFACE_HPP_ */

/****************************************************************************
 * Revision History
 * @format : version/ Date / Author / Change description
 ****************************************************************************
 * Version  Date         Author    Change Log
 * 1        10-16-2017   Revathy   Created Initial file
 *
 ****************************************************************************/
