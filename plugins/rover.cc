#ifndef ROVER_PLUGIN
#define ROVER_PLUGIN

// Include Gazebo headers.
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>

// Include ROS headers so we can communicate with our robot
#include <ros/ros.h>

#include "geometry_msgs/Twist.h"

// Include std::string's because they're pretty darn useful.
#include <string>

#include <cmath>

#define PI 3.14159265

#define SGN(a) (a >= 0 ? 1.0 : -1.0)

#define MAX_DIF_V 0.02
#define DIF_MAX_EFFORT 5

#define MAX_LIN_V 0.0015
#define LIN_MAX_EFFORT 5

// So we don't have to do gazebo::everything
namespace gazebo
{
  // Defining our plugin class
  class RoverPlugin : public ModelPlugin
  {
    private:
    // Model pointer
    physics::ModelPtr _m;

    // Pointer to our onUpdate callback
    event::ConnectionPtr updateConnection;

    double zeroToTwoPi(double angle)
    {
       double out = std::fmod(angle, 2.0 * PI);
       if (out < 0)
         out = 2.0 * PI + out;

       return out;
    }

    double minAngleAdjust(double a1, double a2)
    {
      return std::atan2(std::sin(a2 - a1), std::cos(a2 - a1));
    }

    double pLA, pRA;
    math::Vector3 pPos;

    std::unique_ptr<ros::NodeHandle> node;
    ros::Subscriber *sub;

    double lc_V;
    double lc_Y;

    double pHA;

    public:
    RoverPlugin() {}

    void onCmd(geometry_msgs::Twist t)
    {
       lc_V = t.linear.x;
       lc_Y = t.angular.z;
    }

    // Runs each nanosecond tick
    void onUpdate(const common::UpdateInfo &inf)
    {
       double angleSetpoint = lc_Y * PI / 2.5;
       double lAngle = zeroToTwoPi(_m->GetJoint("lDifJoint")->GetAngle(0).Radian());
       double rAngle = zeroToTwoPi(_m->GetJoint("rDifJoint")->GetAngle(0).Radian());

       double hatAngle = zeroToTwoPi(_m->GetJoint("lazRot")->GetAngle(0).Radian());

       double lAV = minAngleAdjust(pLA, lAngle);
       double rAV = minAngleAdjust(pRA, rAngle);
       double hAV = minAngleAdjust(pHA, hatAngle);

       double lDif = minAngleAdjust(lAngle, angleSetpoint);
       double rDif = minAngleAdjust(rAngle, angleSetpoint);

       double lAd = (((lDif/PI) * MAX_DIF_V - lAV)/MAX_DIF_V) * DIF_MAX_EFFORT;
       double rAd = (((rDif/PI) * MAX_DIF_V - rAV)/MAX_DIF_V) * DIF_MAX_EFFORT;

       double hAd = 0.3 * (0.0075 - hAV)/0.0075;
       _m->GetJoint("lDifJoint")->SetForce(0, lAd);
       _m->GetJoint("rDifJoint")->SetForce(0, rAd);
       _m->GetJoint("lazRot")->SetForce(0, hAd);

       math::Vector3 pos = _m->GetWorldPose().pos;
       math::Quaternion rot = _m->GetWorldPose().rot;
       math::Vector3 dist = pos - pPos;
       double vel = SGN(dist.x * cos(rot.GetYaw()) + dist.y * sin(rot.GetYaw())) * sqrt(dist.x * dist.x + dist.y * dist.y);

       double desLinV = lc_V * MAX_LIN_V;

       _m->GetJoint("jFL")->SetForce(0, -1.0 * LIN_MAX_EFFORT * (desLinV - vel)/MAX_LIN_V);
       _m->GetJoint("jFR")->SetForce(0, -1.0 * LIN_MAX_EFFORT * (desLinV - vel)/MAX_LIN_V);

       //ROS_INFO("V: %f | R: %f | L: %f", vel, rAd, lAd);

       pLA = lAngle;
       pRA = rAngle;
       pHA = hatAngle;
       pPos = pos;
    }

    // Runs when the model is loaded
    virtual void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf)
    {
       if(!ros::isInitialized)
       {
         ROS_FATAL_STREAM("ROS node for Gazebo not established. Plugin failed.");
         return;
       }
       ROS_INFO("Rover Plugin Loaded");

       _m = _model;

       pLA = pRA = pHA = 0;
       lc_V = lc_Y = 0;

       std::string name = _m->GetName();
       node.reset(new ros::NodeHandle(name.c_str()));
       sub = new ros::Subscriber();
       std::string subName = name + "/cmd";
       *sub = node->subscribe(subName, 1, &RoverPlugin::onCmd, this);

       // Bind our onUpdate function to the update callback
       this->updateConnection = event::Events::ConnectWorldUpdateBegin(boost::bind(&RoverPlugin::onUpdate, this, _1));
    }
  };

  // Gazebo macro to set up the rest of the plugin functionality
  GZ_REGISTER_MODEL_PLUGIN(RoverPlugin)
}

#endif


