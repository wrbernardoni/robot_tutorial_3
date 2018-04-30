#ifndef LAZ_SENSOR_PLUGIN
#define LAS_SENSOR_PLUGIN

// Include Gazebo headers.
#include <gazebo/gazebo.hh>
#include <gazebo/plugins/RayPlugin.hh>
#include <gazebo/sensors/Sensor.hh>
#include <gazebo/sensors/SensorTypes.hh>
#include <gazebo/sensors/RaySensor.hh>

// Include ROS headers so we can communicate with our robot
#include <ros/ros.h>

// Include std::string's because they're pretty darn useful.
#include <string>

// So we don't have to do gazebo::everything
namespace gazebo
{
  // Defining our plugin class
  class LazSensorPlugin : public RayPlugin
  {
    private:
    sensors::RaySensorPtr _sensor;

    public:
    LazSensorPlugin() {}

    // Runs when the model is loaded
    virtual void Load(sensors::SensorPtr _s, sdf::ElementPtr _sdf)
    {
       if(!ros::isInitialized)
       {
         ROS_FATAL_STREAM("ROS node for Gazebo not established. Plugin failed.");
         return;
       }
       RayPlugin::Load(_s, _sdf);

       _sensor = std::dynamic_pointer_cast<sensors::RaySensor>(_s);

       ROS_INFO("Laser Sensor Plugin Loaded");
       ROS_INFO("Sensor Parent name: %s", _sensor->ParentName().c_str());
    }

    virtual void OnNewLaserScans()
    {
      double yaw = _sensor->Pose().Rot().Yaw();
      std::string out = "";
      
      ROS_INFO("[%f] Range: %d Ray: %d", yaw, _sensor->RangeCount(), _sensor->RayCount()); 
    };
   };
  

  // Gazebo macro to set up the rest of the plugin functionality
  GZ_REGISTER_SENSOR_PLUGIN(LazSensorPlugin)
}

#endif

