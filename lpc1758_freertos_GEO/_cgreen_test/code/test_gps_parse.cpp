#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

using namespace cgreen;

// The file under test
//#include "NAZA_wrapper/gps_compass_wrapper.h"
#include "NAZA_GPS/NazaDecoder.cpp"
#include "NAZA_GPS/Magnetometer.cpp"
#include "NAZA_GPS/Gps.cpp"
NazaDecoder naza = NazaDecoder();
Ensure(getCorrectmessageId)
{
    naza.decode(0x55);
    naza.decode(0xAA);
    naza.decode(0x20);
    naza.decode(0x06);
    naza.decode(0xAF);
    naza.decode(0xBF);
    naza.decode(0xB8);
    naza.decode(0xBE);
    naza.decode(0x96);
    naza.decode(0xBD);
    naza.decode(0x5D);
    assert_equal(naza.decode(0x02),32);
}
Ensure(getCorrectheader)
{
    assert_equal(naza.decode(0x55),0);
    assert_equal(naza.decode(0xAA),0);
}
Ensure(getCorrect_messageid_notfullpattern)
{
    naza.decode(0x55);
    naza.decode(0xAA);
    assert_equal(naza.decode(0x99),0);
}
Ensure(messageIdillegalvalues)
{

    assert_equal(naza.decode(0x01),0);
    assert_equal(naza.decode(0xAA),0);
    assert_equal(naza.decode(0x11),0);
    assert_equal(naza.decode(0x00),0);
    assert_equal(naza.decode(0x11),0);
    assert_equal(naza.decode(0x99),0);
    assert_equal(naza.decode(0x11),0);
    assert_equal(naza.decode(0x11),0);
    assert_equal(naza.decode(0x44),0);
    assert_equal(naza.decode(0x55),0);
    assert_equal(naza.decode(0x66),0);
    assert_equal(naza.decode(0x77),0);
    assert_equal(naza.decode(0x88),0);
    assert_equal(naza.decode(0x99),0);

    /*for checksum1*/
       naza.decode(0x55);
       naza.decode(0xAA);
       naza.decode(0x20);
       naza.decode(0x06);
       naza.decode(0xAF);
       naza.decode(0xBF);
       naza.decode(0xB8);
       naza.decode(0xBE);
       naza.decode(0x96);
       naza.decode(0xBD);
       naza.decode(0x4D);
       assert_equal(naza.decode(0x02),0);
       /*for checksum2*/
        naza.decode(0x55);
        naza.decode(0xAA);
        naza.decode(0x20);
        naza.decode(0x06);
        naza.decode(0xAF);
        naza.decode(0xBF);
        naza.decode(0xB8);
        naza.decode(0xBE);
        naza.decode(0x96);
        naza.decode(0xBD);
        naza.decode(0x5d);
        assert_equal(naza.decode(0x01),0);

}
Ensure(firmwareversion)
{
           naza.decode(0x55);
           naza.decode(0xAA);
           naza.decode(0x30);
           naza.decode(0x0c);
           naza.decode(0x00);
           naza.decode(0x00);
           naza.decode(0x00);
           naza.decode(0x00);
           naza.decode(0x01);
           naza.decode(0x01);
           naza.decode(0x01);
           naza.decode(0x01);
           naza.decode(0x01);
           naza.decode(0x01);
           naza.decode(0x01);
           naza.decode(0x01);
           naza.decode(0x44);
           assert_equal(naza.decode(0x88),0);

}
//55 AA 30 0C XX XX XX XX FW FW FW FW HW HW HW HW CS CS

TestSuite* geo_controller_test_suite()
{
    TestSuite *geoControllerTestSuite = create_test_suite();
    add_test(geoControllerTestSuite, getCorrectheader);
    add_test(geoControllerTestSuite, getCorrect_messageid_notfullpattern);
    add_test(geoControllerTestSuite, getCorrectmessageId);
    add_test(geoControllerTestSuite, messageIdillegalvalues);
    add_test(geoControllerTestSuite, firmwareversion);
   // add_test(geoControllerTestSuite,              )
    return geoControllerTestSuite;
}
