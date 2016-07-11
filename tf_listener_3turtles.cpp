#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Spawn.h>
#include <std_msgs/String.h>
#include <sstream>

int main(int argc, char** argv){
	ros::init(argc, argv,"my_tf_listener");

	ros::NodeHandle node;

    ros::service::waitForService("spawn");//turtle1
    ros::ServiceClient add_turtle = node.serviceClient<turtlesim::Spawn>("spawn");
    
	turtlesim::Spawn srv;
    add_turtle.call(srv);//request turtle2
    add_turtle.call(srv);//request turtle3

    ros::Publisher turtle2_vel = node.advertise<geometry_msgs::Twist>("turtle2/cmd_vel", 10);
    ros::Publisher turtle3_vel = node.advertise<geometry_msgs::Twist>("turtle3/cmd_vel", 10);

    tf::TransformListener listener;

	ros::Rate rate(10.0);

	while(node.ok()){
        tf::StampedTransform transform1, transform2;
		try{
			ros::Time now = ros::Time::now();
            ros::Time past = now - ros::Duration(3.0);//3s delay

            //turtle1 -> turtle2(no delay)
            listener.waitForTransform("/turtle2", now, "/turtle1", now, "/world", ros::Duration(1.0));
            listener.lookupTransform("/turtle2", now, "/turtle1", now, "/world", transform3);
            //turtle1 -> turtle2(3s delay)
			listener.waitForTransform("/turtle2", now, "/turtle1", past, "/world", ros::Duration(1.0));
            listener.lookupTransform("/turtle2", now, "/turtle1", past, "/world", transform1);
            //turtle2 -> turtle3(3s delay)
            listener.waitForTransform("/turtle3", now, "/turtle2", past, "/world", ros::Duration(1.0));
            listener.lookupTransform("/turtle3", now, "/turtle2", past, "/world", transform2);
		}
		catch(tf::TransformException &ex){
			ROS_ERROR("%s", ex.what());
            ros::Duration(1.0).sleep();
			continue;
		}
        //check the distance
        std_msgs::String msg;
        std::stringstream ss;
        
        double distance = sqrt(pow(transform3.getOrigin().x(), 2) + pow(transform3.getOrigin().y(), 2));
        ss << "The distance between turtle1 and turtle2 is: " << distance;
        msg.data = ss.str();
        ROS_INFO("%s", msg.data.c_str());

        geometry_msgs::Twist vel_msg1, vel_msg2;

        vel_msg1.angular.z = 4.0 * atan2(transform1.getOrigin().y(), transform1.getOrigin().x());
        vel_msg1.linear.x = 0.5 * sqrt(pow(transform1.getOrigin().x(), 2) + pow(transform1.getOrigin().y(), 2));

        vel_msg2.angular.z = 4.0 * atan2(transform2.getOrigin().y(), transform2.getOrigin().x());
        vel_msg2.linear.x = 0.5 * sqrt(pow(transform2.getOrigin().x(), 2) + pow(transform2.getOrigin().y(), 2));

        turtle2_vel.publish(vel_msg1);
        turtle3_vel.publish(vel_msg2);

		rate.sleep();
	}
	return 0;
}
