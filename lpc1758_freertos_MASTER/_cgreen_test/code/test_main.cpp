#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

// Include for C++
using namespace cgreen;

// Include other files under test
#include "test_master_controller.cpp"
#include "mock_definitions.cpp"
#include "test_sensor_io_controller.cpp"


int main(int argc, char **argv) 
{
    TestSuite *suite = create_test_suite();
    add_suite(suite, master_controller_suite());
    add_suite(suite,sensor_controller_suite());

    return run_test_suite(suite, create_text_reporter());
}

