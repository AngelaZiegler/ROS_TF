#include <string>
#include <ros/ros.h>
#include <sensor_msgs/JointState.h>
#include <tf/transform_broadcaster.h>

int main(int argc, char** argv) {
    ros::init(argc, argv, "state_publisher");
    ros::NodeHandle n;
    ros::Publisher joint_pub = n.advertise<sensor_msgs::JointState>("joint_states", 1);
    tf::TransformBroadcaster broadcaster;
    ros::Rate loop_rate(30);

    const double degree = M_PI/180;

    // robot state
    double tilt = 0, tinc = degree, swivel=0, angle=0, rotate=0, rinc=0.008;

    // message declarations
    geometry_msgs::TransformStamped odom_trans;
    sensor_msgs::JointState joint_state;
    odom_trans.header.frame_id = "odom";
    odom_trans.child_frame_id = "axis";

    while (ros::ok()) {
        //update joint_state
        //iteration can be used here but the less efficient way is chosen for individual modification
        joint_state.header.stamp = ros::Time::now();
        joint_state.name.resize(8);
        joint_state.position.resize(8);
        joint_state.name[0] ="j1";
        joint_state.position[0] = swivel;
        joint_state.name[1] ="j2";
        joint_state.position[1] = swivel;
        joint_state.name[2] ="j3";
        joint_state.position[2] = swivel;
        joint_state.name[3] ="j4";
        joint_state.position[3] = swivel;
        joint_state.name[4] ="j5";
        joint_state.position[4] = swivel;
        joint_state.name[5] ="j6";
        joint_state.position[5] = swivel;
        joint_state.name[6] ="ring";
        joint_state.position[6] = rotate;
        joint_state.name[7] ="gunbase";
        joint_state.position[7] = tilt;

        // update transform
        // (moving in a circle with radius=2)
        odom_trans.header.stamp = ros::Time::now();
        odom_trans.transform.translation.x = cos(angle)*(-2);
        odom_trans.transform.translation.y = sin(angle)*(-2);
        odom_trans.transform.translation.z = .7;
        odom_trans.transform.rotation = tf::createQuaternionMsgFromYaw(angle+M_PI);

        //send the joint state and transform
        joint_pub.publish(joint_state);
        broadcaster.sendTransform(odom_trans);

        // Create new robot state
        tilt += tinc;
        if (tilt>.8 || tilt<0) tinc *= -1;//-.5
        rotate += rinc;
        if (rotate>.5 || rotate<-.5) rinc *= -1;
        swivel += (-1) * degree;
        angle += degree/4;

        // This will adjust as needed per iteration
        loop_rate.sleep();
    }


    return 0;
}
