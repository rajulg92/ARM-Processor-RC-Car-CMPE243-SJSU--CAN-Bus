/*
 * LidarTask.hpp
 *
 *  Created on: Oct 28, 2017
 *      Author: Harshitha, Sushma, Supradeep
 */

#ifndef LIDARTASK_HPP_
#define LIDARTASK_HPP_

#include "scheduler_task.hpp"

//FreeRTOS task for the LIDAR
class LIDARtask : public scheduler_task
{
    public:
        LIDARtask(uint8_t priority);
        bool init(void);
        bool run(void *p);
};

#endif /* LIDARTASK_HPP_ */
