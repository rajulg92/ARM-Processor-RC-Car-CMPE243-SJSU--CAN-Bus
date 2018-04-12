#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include "LED_Display.hpp"
#include "LED.hpp"

// Include for C++
using namespace cgreen;

// The file under test
#include "LidarInterfacing.hpp"

RPLidar rp;

Uart3::Uart3() : UartDev((unsigned int*)LPC_UART3_BASE)
{
    //mock();
}

UartDev::UartDev(unsigned int* pUARTBaseAddr) : CharDev(),
        mpUARTRegBase((LPC_UART_TypeDef*) 0),
        mRxQueue(0),
        mTxQueue(0),
        mPeripheralClock(0),
        mRxQWatermark(0),
        mTxQWatermark(0),
        mLastActivityTime(0)
{
    //mock();
}

CharDev::CharDev() : mpPrintfMem(NULL), mPrintfMemSize(0), mReady(false)
{
    //mock();
}

CharDev::~CharDev()
{
   //mock();
}

bool UartDev::getChar(char* pInputChar, unsigned int timeout)
{
    return mock();
}

bool UartDev::putChar(char out, unsigned int timeout)
{
    return mock();
}

bool UartDev::flush(void)
{
    return mock();
}

void LED_Display::setNumber(char x)
{
    mock();
}

void LED::toggle(uint8_t ledNum)
{
    mock();
}

void LED::set(uint8_t ledNum, bool ON)
{
    mock();
}

uint64_t sys_get_uptime_ms(void) { return mock();}

extern "C" uint64_t sys_get_uptime_us(void)
{
    return mock();
}

Ensure(testing_call_to_lidartrack_cond)
{
    assert_equal(rp.trackSelection(-1),13);
    assert_equal(rp.trackSelection(0),1);
    assert_equal(rp.trackSelection(300.56),1);
    assert_equal(rp.trackSelection(450.0),1);
    assert_equal(rp.trackSelection(500.0),1);
    assert_equal(rp.trackSelection(501.56),2);
    assert_equal(rp.trackSelection(800.23),2);
    assert_equal(rp.trackSelection(999),2);
    assert_equal(rp.trackSelection(1000.0),2);
    assert_equal(rp.trackSelection(1000.22),3);
    assert_equal(rp.trackSelection(1200),3);
    assert_equal(rp.trackSelection(1499.55),3);
    assert_equal(rp.trackSelection(1500.0),3);
    assert_equal(rp.trackSelection(1501.0),4);
    assert_equal(rp.trackSelection(1700.99),4);
    assert_equal(rp.trackSelection(2000.0),4);
    assert_equal(rp.trackSelection(2001.11),5);
    assert_equal(rp.trackSelection(2200.66),5);
    assert_equal(rp.trackSelection(2500.0),5);
    assert_equal(rp.trackSelection(2501.0),6);
    assert_equal(rp.trackSelection(2800.0),6);
    assert_equal(rp.trackSelection(3000.0),6);
    assert_equal(rp.trackSelection(3001.0),7);
    assert_equal(rp.trackSelection(3499.0),7);
    assert_equal(rp.trackSelection(3500.0),7);
    assert_equal(rp.trackSelection(3510.33),8);
    assert_equal(rp.trackSelection(3700.99),8);
    assert_equal(rp.trackSelection(4000.0),8);
    assert_equal(rp.trackSelection(4001.0),9);
    assert_equal(rp.trackSelection(4100.77),9);
    assert_equal(rp.trackSelection(4500.0),9);
    assert_equal(rp.trackSelection(4506.0),10);
    assert_equal(rp.trackSelection(4777.66),10);
    assert_equal(rp.trackSelection(5000),10);
    assert_equal(rp.trackSelection(5300.55),11);
    assert_equal(rp.trackSelection(5500.0),11);
    assert_equal(rp.trackSelection(5520.11),12);
    assert_equal(rp.trackSelection(5731.99),12);
    assert_equal(rp.trackSelection(6000.0),12);
    assert_equal(rp.trackSelection(6001.0),13);
    assert_equal(rp.trackSelection(10000.0),13);
    assert_equal(rp.trackSelection(9990.0),13);
}

/*extern "C" int trackSelection(float distance)
{
        return mock();
}*/

Ensure(sector_Selection)
{
    static RPLidarMeasurement buff_test[1];
    static RPLidarRotation rot_test;

    int length = (sizeof(buff_test))/(sizeof(buff_test[0]));

    buff_test[0].distance = 300; buff_test[0].angle = 20; buff_test[0].quality = 0;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector0, 0);

    buff_test[0].distance = 300; buff_test[0].angle = 20; buff_test[0].quality = 0;
    rp.sectorSelection(buff_test, 0, &rot_test);
    assert_equal(rot_test.sector0, 0);

    buff_test[0].distance = 300; buff_test[0].angle = -1; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector0, 0);

    buff_test[0].distance = 300; buff_test[0].angle = 361; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector11, 0);

    buff_test[0].distance = 300; buff_test[0].angle = 0; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector0, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 15; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector0, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 30; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector0, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 31; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector1, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 45; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector1, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 60; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector1, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 61; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector2, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 75; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector2, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 90; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector2, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 91; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector3, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 105; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector3, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 120; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector3, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 121; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector4, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 135; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector4, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 150; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector4, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 151; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector5, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 165; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector5, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 180; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector5, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 181; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector6, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 195; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector6, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 210; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector6, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 211; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector7, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 225; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector7, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 240; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector7, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 241; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector8, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 255; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector8, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 270; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector8, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 271; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector9, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 285; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector9, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 300; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector9, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 301; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector10, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 315; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector10, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 330; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector10, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 331; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector11, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 345; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector11, 1);

    buff_test[0].distance = 300; buff_test[0].angle = 360; buff_test[0].quality = 30;
    rp.sectorSelection(buff_test, length, &rot_test);
    assert_equal(rot_test.sector11, 1);
}

TestSuite *Lidar_track_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, testing_call_to_lidartrack_cond);
    add_test(suite, sector_Selection);

    return suite;
}
