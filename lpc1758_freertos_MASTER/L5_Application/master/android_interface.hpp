/****************************************************************************
 * system_interface.hpp
 * @file :
 * @created : Oct 16,2017
 *
 ****************************************************************************/

#ifndef ANDROID_INTERFACE_HPP_
#define ANDROID_INTERFACE_HPP_

class IAndroidController
{
    public:
    IAndroidController();

    /*
     * @brief : User selected controls from android app is received from BLE controller
     *          This updates motor_start member true if the system start command received from user
     */
    bool HandleUserStartStopCmd(void);

    /*member access methods*/
    bool get_sys_start(void);
    void set_sys_start(bool val);

    BLE_START_STOP_CMD_t BleStartStopMsg;
    //BLE_HEARTBEAT_t BleHeartbeatMsg;

    private:
    /*This flag is set/unset only in the below cases
     *
     * Set True:
     *   1. When User press start button in Android app
     *   2. When user press on board button1
     *
     * Set False:
     *   1. When User press stop button in android app
     *   2. When Master sends Tx_MASTER_SYS_STOP_CMD() while fault handling
     *      Possible system faults :
     *      1. When any/all of the controller's heart beat is not received
     *      2. When sensor mia is handled //ToDo
     *      3. When Bluetooth connection to android app is lost //ToDo BLe should send user stop command messag
     *      4. When user press onboard button2 (manual stop) //ToDo
     *
     * */
    bool msys_start;
};




#endif /* ANDROID_INTERFACE_HPP_ */

/****************************************************************************
 * Revision History
 * @format : version/ Date / Author / Change description
 ****************************************************************************
 * Version  Date         Author    Change Log
 * 1        10-16-2017   Revathy   Created Initial file
 *
 ****************************************************************************/
