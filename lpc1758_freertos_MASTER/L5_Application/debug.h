/*
 * debug.h
 *
 *  Created on: Oct 20, 2017
 *      Author: Revthy
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "printf_lib.h"

// #define DEBUG_E in your c/cpp file before #include<debug_e.h> to enable debug logs
#ifdef DEBUG_E
#define LOGD(fmt , ...) \
    u0_dbg_printf("[DEBUG %s %s %d] " fmt, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#else
#define LOGD(...)
#endif

// #define VERBOSE_E in your c/cpp file before #include<debug_e.h> to enable debug logs
#ifdef VERBOSE_E
#define LOGV(fmt , ...) \
    u0_dbg_printf("[INFO %s %s %d] " fmt, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define LOGV(...)
#endif

//error shall be ON always
#define LOGE(fmt , ...) \
    u0_dbg_printf("[ERROR %s %s %d] " fmt, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#endif /* DEBUG_H_ */
