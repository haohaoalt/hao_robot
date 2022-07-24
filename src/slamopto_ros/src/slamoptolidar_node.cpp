/*
 *  SLAMOPTO ROS Node Client
 *
 *  
 *  
 *
 */

#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include <vector>
#include <iostream>
#include <string>
#include <signal.h>
#include <boost/thread/thread.hpp>
#include "slidar.hpp"

std::string channel_type; // tcp
std::string tcp_ip;		  // 192.168.0.1
int tcp_port;			  // 2111
std::string frame_id;	  // laser"/>
bool inverted;			  // false
bool angle_compensate;	  // ture-->false

double angle_min_d;
double angle_max_d;

double range_min_m;
double range_max_m;

#define PI 3.1415926
float ranges[DATA_NUM];
float intensities[DATA_NUM];
int getFrame = 0;
void tr(void *)
{
	Slidar_M slm(tcp_ip, tcp_port);
	while (ros::ok())
	{
		if (slm.read_dis(ranges, intensities))
		{
			getFrame = 1;
		}
	}
}

int main(int argc, char *argv[])
{

	ros::init(argc, argv, "slamoptolidar_node");
	ros::NodeHandle n;
	ros::Publisher scan_pub = n.advertise<sensor_msgs::LaserScan>("scan", 1);
	ros::Rate rate(200);

	unsigned int num_readings; //= DATA_NUM;
	double laser_frequency = 25;
	float angle_increment = 0.33 * PI / 180;
	float *inner_ranges;	  //[DATA_NUM];
	float *inner_intensities; //[DATA_NUM];

	int begin_pos;
	int end_pos;
	ros::NodeHandle nh_private("~");
	nh_private.param<std::string>("tcp_ip", tcp_ip, "192.168.0.1");
	nh_private.param<int>("tcp_port", tcp_port, 2111);
	nh_private.param<std::string>("frame_id", frame_id, "base_link");
	nh_private.param<bool>("inverted", inverted, "false");
	nh_private.param<bool>("angle_compensate", angle_compensate, "false");

	nh_private.param<double>("anglg_min_d", angle_min_d, -135.0);
	nh_private.param<double>("angle_max_d", angle_max_d, 135.0);
	nh_private.param<double>("range_min_m", range_min_m, 0.1);
	nh_private.param<double>("range_max_m", range_max_m, 25.0);

	begin_pos = (int)((angle_min_d + 135) * DATA_NUM / 270);
	end_pos = (int)((angle_max_d + 135) * DATA_NUM / 270);
	num_readings = end_pos - begin_pos;
	inner_ranges = new float[num_readings]();
	inner_intensities = new float[num_readings]();

	boost::thread thread_r(tr, (void *)NULL);

	if (true == angle_compensate)
	{
		angle_increment = -1 * angle_increment;
	}

	while (ros::ok())
	{
		if (getFrame)
		{
			getFrame = 0;
			sensor_msgs::LaserScan scan_msg;
			ros::Time start_scan_time = ros::Time::now();

			scan_msg.header.stamp = start_scan_time;

			scan_msg.header.frame_id = frame_id;

			scan_msg.angle_min = angle_min_d * PI / 180.0;
			scan_msg.angle_max = angle_max_d * PI / 180.0;

			scan_msg.angle_increment = angle_increment + 0.000074;

			// scan_msg.scan_time = scan.config.scan_time;
			scan_msg.time_increment = (1.0 / laser_frequency) / (DATA_NUM);
			;
			scan_msg.range_min = range_min_m;
			scan_msg.range_max = range_max_m;

			scan_msg.ranges.resize(num_readings);
			scan_msg.intensities.resize(num_readings);

			memcpy(inner_ranges, ranges + begin_pos, num_readings * sizeof(float));
			memcpy(inner_intensities, intensities + begin_pos, num_readings * sizeof(float));

			for (unsigned int i = 0; i < num_readings; ++i)
			{
				if (false == inverted)
				{
					scan_msg.ranges[i] = inner_ranges[i];
					scan_msg.intensities[i] = inner_intensities[i];
				}
				else
				{
					scan_msg.ranges[i] = inner_ranges[num_readings - 1 - i];
					scan_msg.intensities[i] = inner_intensities[num_readings - 1 - i];
				}
			}
			// cout<< "begin_pos="<<begin_pos<<"\tend_pos="<<end_pos<<"\tnum_readings"<<num_readings<<endl;
			// cout << "step lem"(ranges+1) - ranges
			scan_pub.publish(scan_msg);
		}
		rate.sleep();
		ros::spinOnce();
	}

	thread_r.join();
	delete[] inner_ranges;
	delete[] inner_intensities;

	return 0;
}
