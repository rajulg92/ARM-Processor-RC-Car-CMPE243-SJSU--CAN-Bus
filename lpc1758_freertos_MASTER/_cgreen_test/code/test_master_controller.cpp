/**********************************************************
 * test_master_controller.cpp
 *
 * @file : Unit tests for master_controller.cpp
 *  Created on: Oct 28, 2017
 *      Author: Revthy
 **********************************************************/
#include "master_controller.hpp"
#include "i2c2.hpp"
#include "LED_Display.hpp"
#include "LED.hpp"
#include "printf_lib.h"


static MasterController::obs_detection_t obs_status= {0};
static MasterController* pmaster = new MasterController;
static MasterController::steer_dir_t expected_steer = MasterController::steer_straight;


#define MC MasterController

void mock_obstacle_detections(MasterController::steer_dir_t exp_steer,MasterController::steer_dir_t targt_steer,bool fright,bool fleft,bool rright,bool rleft,bool bfornt, bool brear, bool clear);



void mock_obstacle_detections(MasterController::steer_dir_t exp_steer,
                              MasterController::steer_dir_t targt_steer,
                              bool fright,
                              bool fleft,
                              bool rright,
                              bool rleft,
                              bool bfront,
                              bool brear,
                              bool clear)
{
    pmaster->set_target_steer(targt_steer);
    obs_status.frontleft = fleft;
    obs_status.frontright = fright;
    obs_status.rearleft = rleft;
    obs_status.rearright = rright;
    obs_status.no_obstacle = clear;
    obs_status.front = bfront;
    obs_status.rear = brear;
    expected_steer = exp_steer;
}

//Test suites
Ensure(test_obstacle_avoidance)
{
    //Expected steer , target steer , fr , fl , rr, rl ,front, rear,clear

    //Test cases for target steer = steer_right
    pmaster->set_target_steer(MC::steer_right);

    mock_obstacle_detections(MC::steer_right,MC::steer_right,false,false,false,false,false,false,true);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));

    mock_obstacle_detections(MC::steer_straight,MC::steer_right,true,false,false,false,false,false,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));

    mock_obstacle_detections(MC::steer_left,MC::steer_right,true,false,false,false,true,false,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));

    mock_obstacle_detections(MC::steer_straight,MC::steer_right,true,true,false,false,true,false,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(false));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));
    pmaster->set_forward(true);

    mock_obstacle_detections(MC::steer_straight,MC::steer_right,true,true,true,false,true,false,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(false));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));
    pmaster->set_forward(true);

    mock_obstacle_detections(MC::steer_right,MC::steer_right,true,true,true,false,true,true,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::stop));
    pmaster->set_target_speed(MC::speed_slow, true);

    mock_obstacle_detections(MC::steer_right,MC::steer_right,true,true,true,true,true,true,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::stop));
	pmaster->set_target_speed(MC::speed_slow, true);

    //Test cases for target steer = steer_left
    pmaster->set_target_steer(MC::steer_left);

	mock_obstacle_detections(MC::steer_left,MC::steer_left,false,false,false,false,false,false,true);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));

	mock_obstacle_detections(MC::steer_straight,MC::steer_left,false,true,false,false,false,false,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));

	mock_obstacle_detections(MC::steer_right,MC::steer_left,false,true,false,false,true,false,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));

	mock_obstacle_detections(MC::steer_straight,MC::steer_left,true,true,false,false,true,false,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(false));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));
    pmaster->set_forward(true);

	mock_obstacle_detections(MC::steer_left,MC::steer_left,true,true,false,false,true,true,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::stop));
    pmaster->set_target_speed(MC::speed_slow, true);

	mock_obstacle_detections(MC::steer_left,MC::steer_left,true,true,false,true,true,true,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::stop));
    pmaster->set_target_speed(MC::speed_slow, true);

	mock_obstacle_detections(MC::steer_left,MC::steer_left,true,true,true,true,true,true,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::stop));
    pmaster->set_target_speed(MC::speed_slow, true);


    //Test cases for target steer = steer_straight
    pmaster->set_target_steer(MC::steer_straight);

    pmaster->set_forward(true);
	mock_obstacle_detections(MC::steer_straight,MC::steer_straight,false,false,false,false,false,false,true);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));

    mock_obstacle_detections(MC::steer_straight,MC::steer_straight,true,false,false,false,false,false,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));

    mock_obstacle_detections(MC::steer_straight,MC::steer_straight,false,true,false,false,false,false,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));

    mock_obstacle_detections(MC::steer_straight,MC::steer_straight,false,false,true,false,false,false,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));

    mock_obstacle_detections(MC::steer_straight,MC::steer_straight,false,false,false,false,false,true,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));

    mock_obstacle_detections(MC::steer_straight,MC::steer_straight,false,false,false,true,false,false,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));

	mock_obstacle_detections(MC::steer_right,MC::steer_straight,false,false,false,false,true,false,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));

	mock_obstacle_detections(MC::steer_left,MC::steer_straight,true,false,false,false,true,false,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));

	mock_obstacle_detections(MC::steer_straight,MC::steer_straight,true,true,false,false,true,false,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(false));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));
    pmaster->set_forward(true);

	mock_obstacle_detections(MC::steer_straight,MC::steer_straight,true,true,false,false,true,true,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::stop));

    mock_obstacle_detections(MC::steer_straight,MC::steer_straight,true,true,true,false,true,false,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(false));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));
    pmaster->set_forward(true);

    mock_obstacle_detections(MC::steer_straight,MC::steer_straight,true,true,true,true,true,true,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(true));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::stop));

   //Test cases for moving rear
    pmaster->set_target_speed(MC::speed_slow, false);
    pmaster->set_target_steer(MC::steer_straight);

    mock_obstacle_detections(MC::steer_straight,MC::steer_straight,true,true,false,false,true,false,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(false));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));

    mock_obstacle_detections(MC::steer_straight,MC::steer_straight,true,true,true,true,true,false,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(false));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));

    mock_obstacle_detections(MC::steer_right,MC::steer_straight,true,true,false,false,true,true,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(false));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));
    pmaster->set_target_speed(MC::speed_slow, false);

    mock_obstacle_detections(MC::steer_left,MC::steer_straight,true,true,true,false,true,true,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(false));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::speed_slow));
    pmaster->set_target_speed(MC::speed_slow, false);

    mock_obstacle_detections(MC::steer_straight,MC::steer_straight,true,true,true,true,true,true,false);
    assert_that(pmaster->RunObstacleAvoidanceAlgo(obs_status),is_equal_to(expected_steer));
    assert_that(pmaster->get_forward(),is_equal_to(false));
    assert_that(pmaster->get_target_speed(),is_equal_to(MC::stop));
    pmaster->set_target_speed(MC::speed_slow, false);
}

TestSuite* master_controller_suite()
{
    TestSuite* master_suite = create_test_suite();
    add_test(master_suite,test_obstacle_avoidance);

    return master_suite;
}

