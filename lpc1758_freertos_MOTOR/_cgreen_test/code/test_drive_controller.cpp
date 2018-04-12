#include <stdio.h>
#include <stdlib.h>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include "drive_controller.hpp" 
#include "drive_manager.hpp" 

#include "i2c2.hpp"
#include "LED_Display.hpp"
#include "LED.hpp"
#include "printf_lib.h"

#include "eint.h"

using namespace cgreen;

#if 0
class TestDriveController : public DriveController
{
    void testGetDutyCycle()
    {
    assert_equal(getDutyCycle(12.0, ESC_FULL_REV, ESC_NEUTRAL), 0.3);
    }
}
#endif

PWM::PWM(pwmType pwm, unsigned int frequencyHz) : mPwm(pwm)
{
    //mock();
}

PWM::~PWM()
{
    //mock();
}

bool PWM::set(float af)
{
    return true;
}

#if 1
//CAN
#include "can.h"
bool CAN_init(can_t can, uint32_t baudrate_kbps, uint16_t rxq_size, uint16_t txq_size,
              can_void_func_t bus_off_cb, can_void_func_t data_ovr_cb)
{
    return mock();
}

bool CAN_tx (can_t can, can_msg_t *pCanMsg, uint32_t timeout_ms)
{
    return mock();
}

bool CAN_rx (can_t can, can_msg_t *pCanMsg, uint32_t timeout_ms)
{
    return mock();
}

bool CAN_is_bus_off(can_t can)
{
    return mock();
}

void CAN_reset_bus(can_t can)
{

}

uint16_t CAN_get_rx_count(can_t can)
{
    return mock();
}

uint16_t CAN_get_rx_dropped_count(can_t can)
{
    return mock();
}

void CAN_bypass_filter_accept_all_msgs(void)
{
    mock();
}

I2C2::I2C2() : I2C_Base((LPC_I2C_TypeDef*)0)
{
    mock();
}

I2C_Base::I2C_Base(LPC_I2C_TypeDef* pI2CBaseAddr) :
        mpI2CRegs(pI2CBaseAddr),
        mDisableOperation(false)
{
    mock();
}

void LED_Display::setNumber(char a)
{
    mock();
}

void LED::toggle(uint8_t ledNum)
{
    mock();
}

int u0_dbg_printf(const char *format, ...)
{
    return mock();
}

void scheduler_add_task(scheduler_task* task)
{
    mock();
}

//Queue
BaseType_t xQueueGenericSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, const BaseType_t xCopyPosition )
{
    return 0;
}
BaseType_t xQueueGenericReceive( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait, const BaseType_t xJustPeeking )
{
    return 0;
}
QueueHandle_t xQueueGenericCreate( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, const uint8_t ucQueueType )
{
    return 0;
}
void vTaskDelay( const TickType_t xTicksToDelay )
{
    mock();
}

scheduler_task::scheduler_task(const char *name, uint32_t stack, uint8_t priority, void *param) :
#if (0 != configUSE_QUEUE_SETS)
   mQueueSet(0),
   mQueueSetType(0),
   mQueueSetBlockTime(1000),
#endif
   mHandle(0),
   mFreeStack(0),
   mRunCount(0),
   mTaskDelayMs(0),
   mStatUpdateRateMs(60 * 1000),
   mName(name),
   mParam(param),
   mStackSize(stack),
   mPriority(priority)
{
}
#endif

void GPIO::enablePullDown()
{
}

void eint3_enable_port0(uint8_t pin_num, eint_intr_t type, void_func_t func)
{
}

void LED::set(uint8_t ledNum, bool ON)
{
}

GPIO::GPIO(LPC1758_GPIO_Type gpioId) :
    mPortNum(0),
    mPinNum(0) // Should result in (gpioId & 0x1F) if __PNSB is value of 5
{
}

GPIO::~GPIO()
{
}

Ensure(test_getDutyCycle)
{
    DriveController dc = DriveController::getInstance();
    
    /** duty cycle */
    //illegal values
    assert_equal(dc.getDutyCycle(-1.0, 0.0, 0.0, true), 0.0);
    assert_equal(dc.getDutyCycle(10.0, 1.0, 0.0, true), 0.0);
    assert_equal(dc.getDutyCycle(101.0, 0.0, 0.0, true), 0.0);
    assert_equal(dc.getDutyCycle(-1.0, 0.0, 0.0, false), 0.0);
    assert_equal(dc.getDutyCycle(10.0, 1.0, 0.0, false), 0.0);
    assert_equal(dc.getDutyCycle(101.0, 0.0, 0.0, false), 0.0);

    //legal
    assert_equal(dc.getDutyCycle(0.0, 0.0, 0.0, true), 0.0);
    assert_equal(dc.getDutyCycle(0.0, 0.0, 0.0, false), 0.0);
    assert_equal(dc.getDutyCycle(100.0, ESC_NEUTRAL, ESC_FULL_FWD, true), ESC_FULL_FWD);
    assert_equal(dc.getDutyCycle(12.0, ESC_NEUTRAL, ESC_FULL_FWD, true), 7.8);
    assert_equal(dc.getDutyCycle(100.0, ESC_FULL_REV, ESC_NEUTRAL, false), ESC_FULL_REV);
    assert_equal(dc.getDutyCycle(12.0, ESC_FULL_REV, ESC_NEUTRAL, false), 7.2);

    /** fwd and rev */
    //illegal values
    assert_equal(dc.getESCFwdPWMDutyCycle(101.0), 0.0);
    assert_equal(dc.getESCFwdPWMDutyCycle(-1.0), 0.0);
    assert_equal(dc.getESCRevPWMDutyCycle(101.0), 0.0);
    assert_equal(dc.getESCRevPWMDutyCycle(-1.0), 0.0);
    assert_equal(dc.getServoLeftPWMDutyCycle(101.0), 0.0);
    assert_equal(dc.getServoLeftPWMDutyCycle(-1.0), 0.0);
    assert_equal(dc.getServoRightPWMDutyCycle(101.0), 0.0);
    assert_equal(dc.getServoRightPWMDutyCycle(-1.0), 0.0);
    //legal values:
    assert_equal(dc.getESCFwdPWMDutyCycle(0.0), ESC_NEUTRAL);
    assert_equal(dc.getESCFwdPWMDutyCycle(100.0), ESC_FULL_FWD);
    assert_equal(dc.getESCFwdPWMDutyCycle(12.0), 7.8);

    assert_equal(dc.getESCRevPWMDutyCycle(0.0), ESC_NEUTRAL);
    assert_equal(dc.getESCRevPWMDutyCycle(100.0), ESC_FULL_REV);
    assert_equal(dc.getESCRevPWMDutyCycle(12.0), 7.2);

    assert_equal(dc.getServoRightPWMDutyCycle(0.0), SERVO_STRAIGHT);
    assert_equal(dc.getServoRightPWMDutyCycle(100.0), SERVO_HARD_RIGHT);
    assert_equal(dc.getServoRightPWMDutyCycle(12.0), 7.8);

    assert_equal(dc.getServoLeftPWMDutyCycle(0.0), SERVO_STRAIGHT);
    assert_equal(dc.getServoLeftPWMDutyCycle(100.0), SERVO_HARD_LEFT);
    assert_equal(dc.getServoLeftPWMDutyCycle(12.0), 7.2);
}

#define assert_getDriveCmdInfo_result( \
    fSpeedPercent, fSteerPercent, \
    driveSpeedCmd, driveSteerCmd, \
    fSpeedPercentExp, fSteerPercentExp, \
    driveSpeedCmdExp, \
    driveSteerCmdExp \
    ) \
    assert_equal(fSpeedPercent, fSpeedPercentExp); \
    assert_equal(fSteerPercent, fSteerPercentExp); \
    assert_equal(driveSpeedCmd, driveSpeedCmdExp); \
    assert_equal(driveSteerCmd, driveSteerCmdExp)


Ensure(test_getDriveCmdInfo)
{
    DriveManager dm = DriveManager();
    MASTER_SPEED_STEER_CMD_t speedSteer = {0};
    float fSpeedPercent, fSteerPercent;
    eDriveSpeedCmd driveSpeedCmd;
    eDriveSteerCmd driveSteerCmd;

    //legal value; 0's
    dm.getDriveCmdInfo(speedSteer, fSpeedPercent, fSteerPercent, 
        driveSpeedCmd, driveSteerCmd);
    assert_getDriveCmdInfo_result(fSpeedPercent, fSteerPercent, driveSpeedCmd, driveSteerCmd,
            0.0, 0.0, kDriveSpeedCmd_Neutral, kDriveSteerCmd_Straight);

    //illegal values
    speedSteer.MASTER_SPEED_STEER_CMD_speed = 101;
    speedSteer.MASTER_SPEED_STEER_CMD_steer = 101;
    dm.getDriveCmdInfo(speedSteer, fSpeedPercent, fSteerPercent, 
        driveSpeedCmd, driveSteerCmd);
    assert_getDriveCmdInfo_result(fSpeedPercent, fSteerPercent, driveSpeedCmd, driveSteerCmd,
            0.0, 0.0, kDriveSpeedCmd_Invalid, kDriveSteerCmd_Invalid);

    speedSteer.MASTER_SPEED_STEER_CMD_speed = -101;
    speedSteer.MASTER_SPEED_STEER_CMD_steer = -101;
    dm.getDriveCmdInfo(speedSteer, fSpeedPercent, fSteerPercent, 
        driveSpeedCmd, driveSteerCmd);
    assert_getDriveCmdInfo_result(fSpeedPercent, fSteerPercent, driveSpeedCmd, driveSteerCmd,
            0.0, 0.0, kDriveSpeedCmd_Invalid, kDriveSteerCmd_Invalid);

    //legal; same values for speed and steer
    speedSteer.MASTER_SPEED_STEER_CMD_speed = 100;
    speedSteer.MASTER_SPEED_STEER_CMD_steer = 100;
    dm.getDriveCmdInfo(speedSteer, fSpeedPercent, fSteerPercent, 
        driveSpeedCmd, driveSteerCmd);
    assert_getDriveCmdInfo_result(fSpeedPercent, fSteerPercent, driveSpeedCmd, driveSteerCmd,
            100.0, 100.0, 
            kDriveSpeedCmd_Fwd, kDriveSteerCmd_Right);

    speedSteer.MASTER_SPEED_STEER_CMD_speed = 0;
    speedSteer.MASTER_SPEED_STEER_CMD_steer = 0;
    dm.getDriveCmdInfo(speedSteer, fSpeedPercent, fSteerPercent, 
        driveSpeedCmd, driveSteerCmd);
    assert_getDriveCmdInfo_result(fSpeedPercent, fSteerPercent, driveSpeedCmd, driveSteerCmd,
            0.0, 0.0, 
            kDriveSpeedCmd_Neutral, kDriveSteerCmd_Straight);

    // Fwd Straight
    speedSteer.MASTER_SPEED_STEER_CMD_speed = 50;
    speedSteer.MASTER_SPEED_STEER_CMD_steer = 0;
    dm.getDriveCmdInfo(speedSteer, fSpeedPercent, fSteerPercent, 
        driveSpeedCmd, driveSteerCmd);
    assert_getDriveCmdInfo_result(fSpeedPercent, fSteerPercent, driveSpeedCmd, driveSteerCmd,
            50.0, 0.0, 
            kDriveSpeedCmd_Fwd, kDriveSteerCmd_Straight);

    // Fwd Right
    speedSteer.MASTER_SPEED_STEER_CMD_speed = 50;
    speedSteer.MASTER_SPEED_STEER_CMD_steer = 50;
    dm.getDriveCmdInfo(speedSteer, fSpeedPercent, fSteerPercent, 
        driveSpeedCmd, driveSteerCmd);
    assert_getDriveCmdInfo_result(fSpeedPercent, fSteerPercent, driveSpeedCmd, driveSteerCmd,
            50.0, 50.0, 
            kDriveSpeedCmd_Fwd, kDriveSteerCmd_Right);

    //legal; speed and steer shall be different (oppo.)
    // Fwd Left
    speedSteer.MASTER_SPEED_STEER_CMD_speed = 50;
    speedSteer.MASTER_SPEED_STEER_CMD_steer = -50;
    dm.getDriveCmdInfo(speedSteer, fSpeedPercent, fSteerPercent, 
        driveSpeedCmd, driveSteerCmd);
    assert_getDriveCmdInfo_result(fSpeedPercent, fSteerPercent, driveSpeedCmd, driveSteerCmd,
            50.0, 50.0, 
            kDriveSpeedCmd_Fwd, kDriveSteerCmd_Left);

    // Rev Straight
    speedSteer.MASTER_SPEED_STEER_CMD_speed = -50;
    speedSteer.MASTER_SPEED_STEER_CMD_steer = 0;
    dm.getDriveCmdInfo(speedSteer, fSpeedPercent, fSteerPercent, 
        driveSpeedCmd, driveSteerCmd);
    assert_getDriveCmdInfo_result(fSpeedPercent, fSteerPercent, driveSpeedCmd, driveSteerCmd,
            50.0, 0.0, 
            kDriveSpeedCmd_Rev, kDriveSteerCmd_Straight);

    // Rev Right
    speedSteer.MASTER_SPEED_STEER_CMD_speed = -50;
    speedSteer.MASTER_SPEED_STEER_CMD_steer = 50;
    dm.getDriveCmdInfo(speedSteer, fSpeedPercent, fSteerPercent, 
        driveSpeedCmd, driveSteerCmd);
    assert_getDriveCmdInfo_result(fSpeedPercent, fSteerPercent, driveSpeedCmd, driveSteerCmd,
            50.0, 50.0, 
            kDriveSpeedCmd_Rev, kDriveSteerCmd_Right);

    // Rev Left
    speedSteer.MASTER_SPEED_STEER_CMD_speed = -50;
    speedSteer.MASTER_SPEED_STEER_CMD_steer = -50;
    dm.getDriveCmdInfo(speedSteer, fSpeedPercent, fSteerPercent, 
        driveSpeedCmd, driveSteerCmd);
    assert_getDriveCmdInfo_result(fSpeedPercent, fSteerPercent, driveSpeedCmd, driveSteerCmd,
            50.0, 50.0, 
            kDriveSpeedCmd_Rev, kDriveSteerCmd_Left);

    // Neutral Left
    speedSteer.MASTER_SPEED_STEER_CMD_speed = 0;
    speedSteer.MASTER_SPEED_STEER_CMD_steer = -50;
    dm.getDriveCmdInfo(speedSteer, fSpeedPercent, fSteerPercent, 
        driveSpeedCmd, driveSteerCmd);
    assert_getDriveCmdInfo_result(fSpeedPercent, fSteerPercent, driveSpeedCmd, driveSteerCmd,
            0.0, 50.0, 
            kDriveSpeedCmd_Neutral, kDriveSteerCmd_Left);

    // Neutral Right
    speedSteer.MASTER_SPEED_STEER_CMD_speed = 0;
    speedSteer.MASTER_SPEED_STEER_CMD_steer = 50;
    dm.getDriveCmdInfo(speedSteer, fSpeedPercent, fSteerPercent, 
        driveSpeedCmd, driveSteerCmd);
    assert_getDriveCmdInfo_result(fSpeedPercent, fSteerPercent, driveSpeedCmd, driveSteerCmd,
            0.0, 50.0, 
            kDriveSpeedCmd_Neutral, kDriveSteerCmd_Right);

}

Ensure(test_computeTelemetry)
{
    DriveManager dm = DriveManager();
    dm.computeTelemetry(0, 100 /** 100ms */);
    assert_equal(dm.getRPS(), 0);
    assert_equal(dm.getRPM(), 0);
    assert_equal(dm.getSpeed(), 0);

    dm.computeTelemetry(1, 100 /** 100ms */);
    assert_equal(dm.getRPS(), ((((float)1.0) * 1000)) / 100);
    assert_equal(dm.getRPM(), dm.getRPS() * 60);
    assert_equal(dm.getSpeed(), (dm.getRPS() * 22.0 * WHEEL_DIAMETER_CM) / (7.0 * 100));

    dm.computeTelemetry(2, 100 /** 100ms */);
    assert_equal(dm.getRPS(), ((((float)2.0) * 1000)) / 100);
    assert_equal(dm.getRPM(), dm.getRPS() * 60);
    assert_equal(dm.getSpeed(), (dm.getRPS() * 22.0 * WHEEL_DIAMETER_CM) / (7.0 * 100));

    dm.computeTelemetry(10000, 100 /** 100ms */);
    assert_equal(dm.getRPS(), ((((float)10000.0) * 1000)) / 100);
    assert_equal(dm.getRPM(), dm.getRPS() * 60);
    assert_equal(dm.getSpeed(), (dm.getRPS() * 22.0 * WHEEL_DIAMETER_CM) / (7.0 * 100));

    dm.computeTelemetry(1501, 100 /** 100ms */);
    assert_equal(dm.getRPS(), ((((float)1501) * 1000)) / 100);
    assert_equal(dm.getRPM(), dm.getRPS() * 60);
    assert_equal(dm.getSpeed(), (dm.getRPS() * 22.0 * WHEEL_DIAMETER_CM) / (7.0 * 100));

    dm.computeTelemetry(15, 100 /** 100ms */);
    assert_equal(dm.getRPS(), ((((float)15) * 1000)) / 100);
    assert_equal(dm.getRPM(), dm.getRPS() * 60);
    assert_equal(dm.getSpeed(), (dm.getRPS() * 22.0 * WHEEL_DIAMETER_CM) / (7.0 * 100));
}

#if 1
/** TODO:
 * Excessive use of magic numbers below;
 * this is because the RPS, speed values are experimentally set
 * */
Ensure(test_mapTraxxasSpeedPercentToRPS)
{
    DriveManager dm = DriveManager();

    /** invalid values */
    dm.mapTraxxasSpeedPercentToRPS(-1);
    assert_equal(dm.getRPSForSpeedPercent(-1), RPS_ZERO);

    dm.mapTraxxasSpeedPercentToRPS(0);
    assert_equal(dm.getRPSForSpeedPercent(0), RPS_ZERO);

    dm.mapTraxxasSpeedPercentToRPS(1);
    assert_equal(dm.getRPSForSpeedPercent(1), RPS_ZERO);

    dm.mapTraxxasSpeedPercentToRPS(12);
    assert_equal(dm.getRPSForSpeedPercent(12), RPS_ZERO);

    dm.mapTraxxasSpeedPercentToRPS(13);
    assert_equal(dm.getRPSForSpeedPercent(13), RPS_NORMAL);

    dm.mapTraxxasSpeedPercentToRPS(19);
    assert_equal(dm.getRPSForSpeedPercent(19), RPS_NORMAL);

    dm.mapTraxxasSpeedPercentToRPS(20);
    assert_equal(dm.getRPSForSpeedPercent(20), RPS_FAST);

    dm.mapTraxxasSpeedPercentToRPS(29);
    assert_equal(dm.getRPSForSpeedPercent(29), RPS_FAST);

    dm.mapTraxxasSpeedPercentToRPS(30);
    assert_equal(dm.getRPSForSpeedPercent(30), RPS_TURBO);

    dm.mapTraxxasSpeedPercentToRPS(31);
    assert_equal(dm.getRPSForSpeedPercent(31), RPS_TURBO);

    dm.mapTraxxasSpeedPercentToRPS(100);
    assert_equal(dm.getRPSForSpeedPercent(100), RPS_TURBO);

    dm.mapTraxxasSpeedPercentToRPS(101);
    assert_equal(dm.getRPSForSpeedPercent(101), RPS_ZERO);

    dm.mapTraxxasSpeedPercentToRPS(10100);
    assert_equal(dm.getRPSForSpeedPercent(10100), RPS_ZERO);
}
#endif

Ensure(test_updateTargetSpeedPercent)
{
    DriveManager dm = DriveManager();

    /** invalid values */
    float fCurrentTSP = dm.getTargetSpeedPercent();
    dm.updateTargetSpeedPercent(-1);
    assert_equal(dm.getTargetSpeedPercent(), fCurrentTSP);

    /** valid values */
    dm.updateTargetSpeedPercent(0);
    assert_equal(dm.getTargetSpeedPercent(), 0);

    dm.updateTargetSpeedPercent(1);
    assert_equal(dm.getTargetSpeedPercent(), 1);

    dm.updateTargetSpeedPercent(100);
    assert_equal(dm.getTargetSpeedPercent(), 100);

    fCurrentTSP = dm.getTargetSpeedPercent();
    dm.updateTargetSpeedPercent(101);
    assert_equal(dm.getTargetSpeedPercent(), fCurrentTSP);
}

Ensure(test_updateDerivedSpeedPercent)
{
    DriveManager dm = DriveManager();

    /** invalid cases */
    dm.driveState.cmd = kDriveSpeedCmd_Neutral | kDriveSteerCmd_Left;
    assert_false(dm.updateDerivedSpeedPercent(0));
    assert_false(dm.updateDerivedSpeedPercent(1));
    assert_false(dm.updateDerivedSpeedPercent(100));
    assert_false(dm.updateDerivedSpeedPercent(101));

    /** valid cases: fwd */
    dm.driveState.cmd = kDriveSpeedCmd_Fwd | kDriveSteerCmd_Left;
    assert_false(dm.updateDerivedSpeedPercent(0));
    assert_false(dm.updateDerivedSpeedPercent(1));
    assert_false(dm.updateDerivedSpeedPercent(ESC_FORWARD_MIN-1));
    assert_false(dm.updateDerivedSpeedPercent(101));
    assert_true(dm.updateDerivedSpeedPercent(ESC_FORWARD_MIN));
    assert_true(dm.updateDerivedSpeedPercent(20));
    assert_true(dm.updateDerivedSpeedPercent(100));

    /** valid cases: rev */
    dm.driveState.cmd = kDriveSpeedCmd_Rev | kDriveSteerCmd_Left;
    assert_false(dm.updateDerivedSpeedPercent(0));
    assert_false(dm.updateDerivedSpeedPercent(1));
    assert_false(dm.updateDerivedSpeedPercent(ESC_REVERSE_MIN-1));
    assert_false(dm.updateDerivedSpeedPercent(101));
    assert_true(dm.updateDerivedSpeedPercent(ESC_REVERSE_MIN));
    assert_true(dm.updateDerivedSpeedPercent(20));
    assert_true(dm.updateDerivedSpeedPercent(100));

}

TestSuite* drive_controller_test_suite()
{
    TestSuite* pMotorCtrlrTestSuite = create_test_suite();
    add_test(pMotorCtrlrTestSuite, test_getDutyCycle);
    add_test(pMotorCtrlrTestSuite, test_getDriveCmdInfo);
    add_test(pMotorCtrlrTestSuite, test_computeTelemetry);
    add_test(pMotorCtrlrTestSuite, test_mapTraxxasSpeedPercentToRPS);
    add_test(pMotorCtrlrTestSuite, test_updateTargetSpeedPercent);
    add_test(pMotorCtrlrTestSuite, test_updateDerivedSpeedPercent);
    return pMotorCtrlrTestSuite;
}
