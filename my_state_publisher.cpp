#include <string>
#include <ros/ros.h>
#include <sensor_msgs/JointState.h>
#include <tf/transform_broadcaster.h>

int main(int argc, char** argv) {
    ros::init(argc, argv, "state_publisher");
    ros::NodeHandle n;
    ros::Publisher joint_pub = n.advertise<sensor_msgs::JointState>("joint_states", 1);//无缓冲
    tf::TransformBroadcaster broadcaster;
    ros::Rate loop_rate(30);

    const double degree = M_PI/180;//角度转弧度系数
    const int pos = 2;
    const int neg = -2;

    // robot state
    double tilt = 0, tinc = degree, swivel=0, angle=0, height=0, hinc=0.005;
    int fx = 200, fy = 0, dx = 0, dy = 0;
    double x = 2, y = 0;
    
    // message declarations
    geometry_msgs::TransformStamped odom_trans;
    sensor_msgs::JointState joint_state;
    odom_trans.header.frame_id = "odom";//关系 odom -> axis
    odom_trans.child_frame_id = "axis";

    while (ros::ok()) {
        //update joint_state
        //这里只有位置，没有速度和受力
        joint_state.header.stamp = ros::Time::now();
        joint_state.name.resize(3);//string自带，变为长度3的字符串
        joint_state.position.resize(3);
        joint_state.name[0] ="swivel";//头关节，控制头转动
        joint_state.position[0] = swivel;
        joint_state.name[1] ="tilt";//身体关节，控制身体俯仰
        joint_state.position[1] = tilt;
        joint_state.name[2] ="periscope";//“潜望镜”，一个与身体无关的部分，可以上下运动
        joint_state.position[2] = height;


        // update transform
        odom_trans.header.stamp = ros::Time::now();
        odom_trans.transform.translation.x = x;
        odom_trans.transform.translation.y = y;
        odom_trans.transform.translation.z = .7;
        odom_trans.transform.rotation = tf::createQuaternionMsgFromYaw(angle);
        //send the joint state and transform
        joint_pub.publish(joint_state);
        broadcaster.sendTransform(odom_trans);

        // Create new robot state
        tilt += tinc;
        if (tilt<-.5 || tilt>0) tinc *= -1;//-.5
        height += hinc;
        if (height>.2 || height<0) hinc *= -1;
        swivel += degree;//机器人头转动速度
        dx = 0;
        dy = 0;
        //使机器人始终面向前方
        if(x == 2){
            angle = M_PI/2.0;
        }
        if(x == -2){
            angle = 3.0*M_PI/2.0;
        }
        if(y == 2){
            angle = M_PI;
        }
        if(y == -2){
            angle = 0;
        }

        if((fx == 200) && (fy != 200)){
            dy = pos;
        }
        if((fx == -200) && (fy != -200)){
            dy = neg;
        }
        if((fy == 200) && (fx != -200)){
            dx = neg;
        }
        if((fy == -200) && (fx != 200)){
            dx = pos;
        }
        fx += dx;
        fy += dy;
        x = (double)fx/100.0;
        y = (double)fy/100.0;

        // This will adjust as needed per iteration
        loop_rate.sleep();
    }

    return 0;
}
