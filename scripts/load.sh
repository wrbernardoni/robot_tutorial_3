#!/bin/bash

cd $(rospack find robot_tutorial_3)/urdf

x-terminal-emulator -e roslaunch gazebo_ros empty_world.launch 

rosrun gazebo_ros spawn_model -file lidar.urdf -urdf -model rover

#rosrun robot_tutorial_1 keyin rover

