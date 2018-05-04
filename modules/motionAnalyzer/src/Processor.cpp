/******************************************************************************
 *                                                                            *
 * Copyright (C) 2018 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

/**
 * @file Processor.cpp
 * @authors: Valentina Vasco <valentina.vasco@iit.it>
 */

#include "Processor.h"

using namespace std;
using namespace yarp::math;
using namespace assistive_rehab;

const string Rom_Processor::motion_type = "ROM";

Processor* createProcessor(const string& motion_tag, const Metric* metric_)
{
    if(motion_tag.compare(Rom_Processor::motion_type) >= 0)
    {
        yInfo() << "Creating processor for" << Rom_Processor::motion_type << "\n";
        return new Rom_Processor(metric_);
    }
    else
        return 0;
}

/********************************************************/
Processor::Processor()
{
    deviation = 0.0;
    invT.resize(4,4);
    invT.zero();
}

void Processor::setInitialConf(const SkeletonWaist &skeleton_init_, const map<string, pair<string, double> > &keypoints2conf_)
{
    skeleton_init.update(skeleton_init_.get_unordered());
    skeleton_init.setTag("init");
    keypoints2conf = keypoints2conf_;

//    skeleton_init.print();
}

bool Processor::isStatic(const KeyPoint& keypoint)
{
    if(keypoints2conf[keypoint.getTag()].first == "static")
        return true;
    else
        return false;
}

/****************************************************************/
void Processor::update(SkeletonWaist &curr_skeleton_)
{
    curr_skeleton.update_fromstd(curr_skeleton_.toProperty());
//    curr_skeleton.print();

    Vector xyz=curr_skeleton[KeyPointTag::shoulder_center]->getPoint();
    Vector xyz_inv=invT.subcol(0,3,3);
    Vector rot(4,0.0);
    Matrix T=axis2dcm(rot);
    T(0,3)=xyz[0];
    T(1,3)=xyz[1];
    T(2,3)=xyz[2];
    invT=SE3inv(T);

    xyz+=xyz_inv;

    T(0,3)=xyz[0];
    T(1,3)=xyz[1];
    T(2,3)=xyz[2];

    skeleton_init.setTransformation(T);
    skeleton_init.update();
//    skeleton_init.print();
//    cout << endl;


}

bool Processor::isDeviatingFromIntialPose()
{
    bool isDeviating = false;
    deviation = 0.0;
    for(unsigned int i=0; i<curr_skeleton.getNumKeyPoints(); i++)
    {
        if(curr_skeleton[i]->isUpdated() && curr_skeleton[i]->getTag() != KeyPointTag::hip_center)
        {
            if(isStatic(*curr_skeleton[i]))
            {
                deviation+= isDeviatingFromIntialPose(*curr_skeleton[i], *skeleton_init[i]);
                isDeviating = true;
//                yWarning() << curr_skeleton[i]->getTag() << "deviating from initial pose";
            }
        }
    }

    return isDeviating;
}

bool Processor::isOutOfSphere(const KeyPoint& keypoint, const KeyPoint& keypoint_init)
{
    Vector curr_kp(keypoint.getPoint()), curr_kp_init(keypoint_init.getPoint());
    double deviation = norm(curr_kp-curr_kp_init);

    yInfo() << keypoint.getTag().c_str() << deviation;
    return (deviation > keypoints2conf[keypoint.getTag()].second);
}

double Processor::isDeviatingFromIntialPose(const KeyPoint& keypoint, const KeyPoint& keypoint_init)
{
    Vector curr_kp(keypoint.getPoint()), curr_kp_init(keypoint_init.getPoint());
//    Vector curr_pose, initial_pose;

//    //if the current keypoint has child and parent
//    if(keypoint.getNumChild() && keypoint.getNumParent())
//    {
//        Vector curr_kp_parent = keypoint.getParent(0)->getPoint();
//        Vector curr_kp_child = keypoint.getChild(0)->getPoint();

//        Vector curr_kp_parent_init = keypoint_init.getParent(0)->getPoint();
//        Vector curr_kp_child_init = keypoint_init.getChild(0)->getPoint();

//        curr_pose = curr_kp + curr_kp_parent + curr_kp_child;
//        initial_pose = curr_kp_init + curr_kp_parent_init + curr_kp_child_init;
//    }
//    else if(keypoint.getNumParent()) //if the current keypoint has parent and not child
//    {
//        Vector curr_kp_parent = keypoint.getParent(0)->getPoint();
//        Vector curr_kp_parent_init = keypoint_init.getParent(0)->getPoint();

//        curr_pose = curr_kp + curr_kp_parent;
//        initial_pose = curr_kp_init + curr_kp_parent_init;
//    }
//    else
//    {
//        curr_pose = curr_kp;
//        initial_pose = curr_kp_init;
//    }

    double dev = norm(curr_kp-curr_kp_init);
//    double dev = norm(curr_pose-initial_pose);
//    Vector dev = curr_pose-initial_pose;
//    double deviation = dev[0]+dev[1]+dev[2];
    //    if(deviation > keypoints2conf[keypoint.getTag()].second)
//    {
//        yInfo() << keypoint.getTag().c_str()
//                << "(" << curr_kp[0] << "," << curr_kp[1] << "," << curr_kp[2] << ")"
//                << "(" << curr_kp_init[0] << "," << curr_kp_init[1] << "," << curr_kp_init[2] << ")"
//    //            << "(" << curr_kp_parent[0] << "," << curr_kp_parent[1] << "," << curr_kp_parent[2] << ")"
//    //            << "(" << curr_kp_child[0] << "," << curr_kp_child[1] << "," << curr_kp_child[2] << ")"
////                << "(" << curr_pose[0] << "," << curr_pose[1] << "," << curr_pose[2] << ")"
////                << "(" << initial_pose[0] << "," << initial_pose[1] << "," << initial_pose[2] << ")"
//                << dev;
//    }

    if(dev > keypoints2conf[keypoint.getTag()].second)
        return dev;
    else
        return 0.0;
}

/********************************************************/
Rom_Processor::Rom_Processor()
{

}

Rom_Processor::Rom_Processor(const Metric *rom_)
{
    rom = (Rom*)rom_;
}

//void Rom_Processor::setInitialConf(const SkeletonWaist &skeleton_init_, const map<string, pair<string, double> > &keypoints2conf_)
//{
//    skeleton_init.update(skeleton_init_.get_unordered());
//    keypoints2conf = keypoints2conf_;
//}

double Rom_Processor::computeMetric(Vector &v1, Vector &plane_normal, Vector &ref_dir)
{
    //get reference keypoint from skeleton
    string tag_joint = rom->getTagJoint();

//    cout << "compute metric for " << tag_joint.c_str() << endl;
    Vector kp_ref = curr_skeleton[tag_joint]->getPoint();

//    Vector v1;
    double theta;
    if(curr_skeleton[tag_joint]->getNumChild())
    {
        Vector kp_child = curr_skeleton[tag_joint]->getChild(0)->getPoint();

        plane_normal = rom->getPlane(); //curr_skeleton.getCoronal();
        ref_dir = rom->getRefDir();

//        yInfo() << "plane" << plane_normal[0] << plane_normal[1] << plane_normal[2];
        v1 = kp_child-kp_ref;

        double dist = dot(v1,plane_normal);
//        yInfo() << "v1 before" << v1[0] << v1[1] << v1[2] << dist;
//        Vector test1 = dist*plane_normal;
//        yInfo() << "test" << test1[0] << test1[1] << test1[2];

        v1 = v1-dist*plane_normal;
//        yInfo() << "v1" << v1[0] << v1[1] << v1[2];
        v1/=norm(v1);

//        yInfo() << "ref" << ref_dir[0] << ref_dir[1] << ref_dir[2];

        double v1_norm = norm(v1);
        double v2_norm = norm(ref_dir);
        double dot_p = dot(v1,ref_dir);

        //        double minval = rom->getMinVal() * (M_PI/180);
        //        double maxval = rom->getMaxVal() * (M_PI/180);
        //        double range = M_PI;
        //        double scale = (maxval-minval)/range;
        //        theta = scale*acos(dot_p/(v1_norm*v2_norm))+minval;
        theta = acos(dot_p/(v1_norm*v2_norm));
//        yInfo() << "v1" << v1[0] << v1[1] << v1[2] << dot_p << theta * (180/M_PI);

        return ( theta * (180/M_PI) );
    }
    else
    {
        yError() << "The keypoint does not have a child ";
        return 0.0;
    }
}
