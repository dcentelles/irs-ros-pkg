/*
 * VispUtils: usage example in http://github.com/davidfornas/pcl_manipulation
 *
 *  Created on: 24/04/2014
 *      Author: dfornas
 */
#ifndef VISPUTILS_H
#define VISPUTILS_H

#include <tf/transform_broadcaster.h>
#include <visp/vpHomogeneousMatrix.h>
#include <string>
#include <visualization_msgs/Marker.h>

struct Frame{
	tf::Transform pose;
	std::string parent, child;
	friend std::ostream& operator<<(std::ostream& out, Frame& x );
};

/** Tool to publish a vpHomogeneousMatrix on the TF tree */
class VispToTF
{

	tf::TransformBroadcaster *broadcaster_;
	std::map<std::string, Frame> frames_;


public:

    /** Create a new publisher with a frame to add to the TF tree */
    VispToTF( vpHomogeneousMatrix sMs, std::string parent, std::string child );
    /** Create a new empty publisher */
    VispToTF();

    /** Add a frame to the publish list */
	void addTransform( vpHomogeneousMatrix sMs, std::string parent, std::string child, std::string id = "0"  );
	
	/** Remove a frame from the publish list */
	void removeTransform( std::string id = "0" );
	
	/** Modify homog matrix of a transform, it's better to keep parent and child unmodified */
	void resetTransform( vpHomogeneousMatrix sMs, std::string id = "0" );
	
	void publish();
	void print();

   // ~VispToTF();
};

std::ostream& operator<<(std::ostream& out, Frame& x ) 
{
	  out << "Pose origin  [x: " << x.pose.getOrigin().x() << " y: " << x.pose.getOrigin().y() << " z: " << x.pose.getOrigin().z() << "] "
	  << "Pose rotation [x:" << x.pose.getRotation().x() << " y:" << x.pose.getRotation().y() << " z:" << x.pose.getRotation().z() << " w:" << x.pose.getRotation().w() << "] "
	  << std::endl << "Parent: " << x.parent << " Child: " << x.child << std::endl;
	  return out;
}

tf::Transform tfTransFromVispHomog( vpHomogeneousMatrix sMs){
	
	  tf::Transform pose;
	
	  vpTranslationVector trans;
	  sMs.extract(trans);
	  tf::Vector3 translation(trans[0],trans[1],trans[2]);

	  vpQuaternionVector rot;
	  sMs.extract(rot);
	  tf::Quaternion rotation( rot.x(), rot.y(), rot.z(), rot.w());
	  pose.setOrigin(translation);   pose.setRotation(rotation);
	  return pose;
	  
}

class VispToMarker
{

	ros::Publisher publisher_;	

public:

     //In ths case is safe to make it public. The it's easier to change  marker's appearance.
	 visualization_msgs::Marker marker;
		

	 VispToMarker( vpHomogeneousMatrix sMs, std::string parent, std::string topic_name, ros::NodeHandle nh){ 
		 setMarker( sMs, parent ); 
		 publisher_ =nh.advertise<visualization_msgs::Marker>(topic_name,1);
	 }
	 
	 void setMarker( vpHomogeneousMatrix sMs, std::string parent, int duration=1 ){
		
		tf::Transform pose=tfTransFromVispHomog(sMs);
		 
		marker.header.frame_id = parent;
		marker.header.stamp = ros::Time::now();
		marker.ns = "pose_marker";
		marker.id = 0;
		marker.type = visualization_msgs::Marker::ARROW; //TODO: More marker types if needed.
		marker.action = visualization_msgs::Marker::ADD;
		marker.pose.position.x = pose.getOrigin().x();
		marker.pose.position.y = pose.getOrigin().y();
		marker.pose.position.z = pose.getOrigin().z();
		marker.pose.orientation.x = pose.getOrigin().x();
		marker.pose.orientation.y = pose.getOrigin().y();
		marker.pose.orientation.z = pose.getOrigin().z();
		marker.pose.orientation.w = pose.getOrigin().w();
		marker.lifetime = ros::Duration(duration);
		changeScale(0.1, 0.1, 0.2);
		changeColor(1, 0.2, 0.7, 0.7);
	  }
	  
	  void publish(){ publisher_.publish(marker); }
	  
	  void changeColor( double r, double g, double b, double a ){
		marker.color.a = r;	marker.color.r = g;
		marker.color.g = b;	marker.color.b = a;
	  }
	  
	  void changeScale( double x, double y, double z){
		marker.scale.x = x; marker.scale.y = y;	marker.scale.z = z;
	  }

};
#endif
