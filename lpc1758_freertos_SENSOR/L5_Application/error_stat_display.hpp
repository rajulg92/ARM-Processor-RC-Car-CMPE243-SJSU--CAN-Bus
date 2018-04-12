/*
 * error_stat_display.hpp
 *
 *  Created on: Oct 17, 2017
 *      Author: unnik
 */

#ifndef ERROR_STAT_DISPLAY_HPP_
#define ERROR_STAT_DISPLAY_HPP_

#include "io.hpp"

typedef enum
{
    kErrorStatDisplay_SSCmdMIAOccured = 1,
    kErrorStatDisplay_BusOff = 3,
    kErrorStatDisplay_LidarFault = 4,
    kErrorStatDisplay_NoData = 2
}eErrorStatDisplay_LED;

//code: ErrorStatDisplay::displayErrorStat(kErrorStatDisplay_BusOff)
class ErrorStatDisplay
{
public:
    static void displayErrorStat(eErrorStatDisplay_LED anErrorLED)
    {
        /** toggling make the display more humanly visible
         * provided next error hits after substantial amount of time
         * Even if error hits repeatedly, LED will appear glowing/ON
         *  */
        LE.toggle(anErrorLED);
    }

    static void clearErrorStat(eErrorStatDisplay_LED anErrorLED)
    {
        LE.set(anErrorLED, false);
    }

    /** Display the integer (0 to 99)
     * on the LED Display
     * Don't use this function for values that change quickly
     * (no point in doing so)
     *  */
    static void displayDebugInt(uint8_t anInt_8b)
    {
        LD.setNumber(anInt_8b);
    }
};


#endif /* ERROR_STAT_DISPLAY_HPP_ */
