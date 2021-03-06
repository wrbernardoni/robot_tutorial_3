cmake_minimum_required(VERSION 2.8.3)
project(robot_tutorial_3)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

# Find dependencies
find_package(catkin REQUIRED COMPONENTS
  gazebo_ros
  roscpp
  geometry_msgs
  std_msgs
  sensor_msgs
  xacro
)
find_package(gazebo REQUIRED)

# Generate Xacro files
file(GLOB xacro_files urdf/*.xacro)
xacro_add_files(${xacro_files} TARGET media_files)

# Declare dependencies
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES robot_tutorial_1
#  CATKIN_DEPENDS gazebo_ros roscpp
#  DEPENDS system_libi
  DEPENDS
    roscpp
    gazebo_ros
    message_runtime
)

# Link to gazebo
link_directories(
  ${GAZEBO_LIBRARY_DIRS}
)

# Include dirs.
include_directories(
# include
  ${catkin_INCLUDE_DIRS}
  ${roscpp_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${GAZEBO_INCLUDE_DIRS}
)

# Compile plugin
 add_library(new_rover_plugin plugins/rover.cc)
 target_link_libraries(new_rover_plugin ${catkin_LIBRARIES} ${GAZEBO_LIBRARIES} ${roscpp_LIBRARIES})

 add_library(laz_sensor_plugin plugins/lazSensor.cc)
 target_link_libraries(laz_sensor_plugin ${catkin_LIBRARIES} ${GAZEBO_LIBRARIES} ${roscpp_LIBRARIES} RayPlugin)


