#!/bin/bash

# 
# This file executes from the build directory by the Eclipse "Pre Build" step
#

#
# Generate the can.h with either of the  follwing module names corresponding to your module
# Module names : MASTER , GEO , SENSOR , BRIDGE , MOTOR
#

python ../../_can_dbc/dbc_parse.py -i ../../_can_dbc/243.dbc -s GEO > ../_can_dbc/generated_can.h