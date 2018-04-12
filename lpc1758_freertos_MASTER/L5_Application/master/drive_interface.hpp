/****************************************************************************
 * drive_interface.hpp
 * @file :
 * @created : Oct 16,2017
 *
 ****************************************************************************/

#ifndef DRIVE_INTERFACE_HPP_
#define DRIVE_INTERFACE_HPP_
#include <stdint.h>

class IDriveController
{
    public:

    /* The motor min speed is set to 12% of Full speed*
     * The enum values represent the % of full speed of motor
     * @notes : with 50Hz(20ms) PWM signal Motor max speed is at 10% of 20ms
     *
     */
    typedef enum
    {
        stop = 0,
        speed_slow = 13,
        speed_normal= 20,
        speed_fast= 30
    } speed_mode_t;

    /*
     * Servo PWM operating conditions are freq = 50Hz (20ms)
     * The enum values are decoded by drive controller based on the following calculation
     * @notes : steer direction of Servo is right= 1ms, left= 2ms, neutral = 1.5ms
     */
    typedef enum
    {
        steer_straight = 0,
        steer_right =1,
        steer_left =2
    }steer_dir_t;

    IDriveController();

    /* member access methods*/
    speed_mode_t get_current_speed(void);
    steer_dir_t get_current_steer(void);
    steer_dir_t get_target_steer(void);
    speed_mode_t get_target_speed(void);
    void set_forward(bool fwd);
    void set_target_speed(speed_mode_t speed,bool isfwd);
    void set_target_steer(steer_dir_t dir);
    void set_target_steer_angle(int16_t turn_angle );
    bool get_forward(void);

    /*updates the CAN message with the new command*/
    void UpdateSpeedSteerSignal(void);

    float getTargetPercent(float afPercent, float afLowBound, float afUpperBound, bool bTowardUpper);

    MASTER_SPEED_STEER_CMD_t SpeedSteerMsg;

    private:

    speed_mode_t mcurr_speed;
    steer_dir_t mcurr_steer;
    speed_mode_t mtarget_speed;
    steer_dir_t mtarget_steer;
    bool mmoving_forward;
    int16_t mgeo_turning_angle;
	const uint32_t mnReverseSpeedOffset;
};





#endif /* DRIVE_INTERFACE_HPP_ */


/****************************************************************************
 * Revision History
 * @format : version/ Date / Author / Change description
 ****************************************************************************
 * Version  Date         Author    Change Log
 * 1        10-16-2017   Revathy   Created Initial file. Added interface file headers
 *                                 for motor,sensor and system
 * 2        10-27-2017   Revathy   Refactoring method names
 ****************************************************************************/
