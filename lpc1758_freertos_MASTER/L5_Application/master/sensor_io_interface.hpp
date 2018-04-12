/****************************************************************************
 * sensor_io_interface.hpp
 * @file :
 * @created : Oct 16,2017
 *
 ****************************************************************************/

#ifndef SENSOR_IO_INTERFACE_HPP_
#define SENSOR_IO_INTERFACE_HPP_

class ISensorController
{
    public:

    /* To be removed
     * enums indicating the presence of obstacles around the car
     *
     */
    typedef enum{
        obs_clear = 0,
        obs_frontright =1,
        obs_frontleft = 2,
        obs_rearleft =3,
        obs_rearright =4, //obstacle in only rear right, so possible movements are rear left or front
        obs_front =5, // obstacle covering whole of front
        obs_rear =6,
        obs_block =7  // obstacle  covering all sides
    }obs_location_t;

    /*
     * @brief: The sector containing obstacle are grouped to this structure members
     *         12 sectors with 12 tracks covers 360 degree field of view and 6 meters range of obstacle data
     */

    typedef struct
    {
        bool frontleft;
        bool frontright;
        bool rearright;
        bool rearleft;
        bool rear;
        bool front;
        bool no_obstacle;
    }obs_detection_t;

    ISensorController();

    /* Sensor obstacle info is packed in this CAN message as sectors and tracks*/
    SENSOR_LIDAR_OBSTACLE_INFO_t LidarObstacleMsg;

    //SENSOR_HEARTBEAT_t  LidarHeartbeatMsg;

    /* Algorithm that runs on sensor obstacle info to group the sectors
     * The 360' view is divided into 12 sectors.so each sector covers 30 degree view
     * Sectors - 0,1,2 - covers front right
     * Sectors - 3,4,5 - covers front left
     * Sectors - 6,7,8 - covers rear left
     * Sectors - 9,10,11 - covers rear right
     */

    obs_detection_t RunObstacleDetectionAlgo(void);

    void set_obstacle_detection(obs_detection_t adetections);

    private:
    obs_location_t mcurrent_obstacle_status; //To be removed
    obs_detection_t mobs_detection;
};



#endif /* SENSOR_IO_INTERFACE_HPP_ */


/****************************************************************************
 * Revision History
 * @format : version/ Date / Author / Change description
 ****************************************************************************
 * Version  Date         Author    Change Log
 * 1        10-16-2017   Revathy   Created Initial file
 * 2        10-27-2017   Revathy   Implemented ObstacleDetectionAlgorithm
 ****************************************************************************/
