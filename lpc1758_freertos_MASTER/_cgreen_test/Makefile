# Compiler flags
CXX = g++
CXXFLAGS = -Wall -g

# Points to the root of cgreen test files
CGREEN_DIR = ../../../cgreen

# Google Test headers
CGREEN_HEADERS = -I$(CGREEN_DIR)/include -I$(CGREEN_DIR)/include/cgreen/internal

# External cgreen library and where it's located
LIBS = -L$(CGREEN_DIR) -lcgreen

# Where to place your unit test code
UT_DIR = code

# SJSU Project file directories
PROJ_DIR = ../../lpc1758_freertos_Master
HEADER_DIR = -I$(PROJ_DIR) \
               -I$(PROJ_DIR)/L0_LowLevel \
               -I$(PROJ_DIR)/L1_FreeRTOS \
               -I$(PROJ_DIR)/L1_FreeRTOS/include \
               -I$(PROJ_DIR)/L1_FreeRTOS/hooks \
               -I$(PROJ_DIR)/L1_FreeRTOS/MemMang \
               -I$(PROJ_DIR)/L1_FreeRTOS/portable \
               -I$(PROJ_DIR)/L1_FreeRTOS/portable/mpu \
               -I$(PROJ_DIR)/L2_Drivers/base \
               -I$(PROJ_DIR)/L2_Drivers/src \
               -I$(PROJ_DIR)/L2_Drivers \
               -I$(PROJ_DIR)/L3_Utils \
               -I$(PROJ_DIR)/L3_Utils/tlm \
               -I$(PROJ_DIR)/L4_IO \
               -I$(PROJ_DIR)/L4_IO/fat \
               -I$(PROJ_DIR)/L4_IO/fat/disk \
               -I$(PROJ_DIR)/L4_IO/fat/option \
               -I$(PROJ_DIR)/L4_IO/wireless \
               -I$(PROJ_DIR)/L5_Application \
               -I$(PROJ_DIR)/L5_Application/master \
               -I$(PROJ_DIR)/L5_Application/examples \
               -I$(PROJ_DIR)/L5_Application/periodic_scheduler \

# Master directory
MASTER_DIR = $(PROJ_DIR)/L5_Application/master

# Default for "make"
all : test_all

clean :
	rm -f test_all *.o

test_main.o: $(UT_DIR)/test_main.cpp
	$(CXX) $(CGREEN_HEADERS) $(HEADER_DIR) $(CXXFLAGS) -c $(UT_DIR)/test_main.cpp $(LIBS)

drive_interface.o: $(MASTER_DIR)/drive_interface.cpp 
	$(CXX) $(CGREEN_HEADERS) $(HEADER_DIR) $(CXXFLAGS) -c  $(MASTER_DIR)/drive_interface.cpp  $(LIBS)

android_interface.o: $(MASTER_DIR)/android_interface.cpp 
	$(CXX) $(CGREEN_HEADERS) $(HEADER_DIR) $(CXXFLAGS) -c  $(MASTER_DIR)/android_interface.cpp  $(LIBS)
	
geo_interface.o: $(MASTER_DIR)/geo_interface.cpp 
	$(CXX) $(CGREEN_HEADERS) $(HEADER_DIR) $(CXXFLAGS) -c  $(MASTER_DIR)/geo_interface.cpp  $(LIBS)
	
sensor_io_interface.o: $(MASTER_DIR)/sensor_io_interface.cpp
	$(CXX) $(CGREEN_HEADERS) $(HEADER_DIR) $(CXXFLAGS) -c  $(MASTER_DIR)/sensor_io_interface.cpp  $(LIBS)
	
master_controller.o: $(MASTER_DIR)/master_controller.cpp
	$(CXX) $(CGREEN_HEADERS) $(HEADER_DIR) $(CXXFLAGS) -c  $(MASTER_DIR)/master_controller.cpp  $(LIBS)
	
test_all: test_main.o master_controller.o drive_interface.o geo_interface.o android_interface.o sensor_io_interface.o
	$(CXX) $(CGREEN_HEADERS) $(HEADER_DIR) $(CXXFLAGS) $^ -o $@ $(LIBS)
