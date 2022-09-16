// Just some sample code from a tutorial
// This is a module I am creating for the Godot game engine
// so that it supports the microsoft kinect.
#include "gkinect.h"


#include <Windows.h>
#include <Ole2.h>

#include <Kinect.h>

#include <vector>

IKinectSensor* sensor = NULL;             // Kinect sensor
IMultiSourceFrameReader* reader = NULL;   // Kinect data source
ICoordinateMapper* mapper = NULL;

BOOLEAN tracked;

//std::vector<Joint*> jointList;

Joint joints[JointType_Count];

std::vector<std::vector<Joint>> bodies;


GKinect* GKinect::singleton;

static bool already_initialized = false;

GKinect::GKinect() {
    singleton = this;
}

GKinect* GKinect::get_singleton()
{
    return singleton;
}

void GKinect::initialize() {
    if (!already_initialized)
    {
        if (FAILED(GetDefaultKinectSensor(&sensor)))
        {
            //Godot::print("Issue getting the kinect sensor.");
            return;
        }

        if (sensor)
        {
            sensor->get_CoordinateMapper(&mapper);
            sensor->Open();

            sensor->OpenMultiSourceFrameReader(FrameSourceTypes::FrameSourceTypes_Body,
                &reader);
        }
        else
        {
            //Godot::print("Issue grabbing the sensor...");
            return;
        }
        already_initialized = true;
        std::vector<Joint> j(JointType_Count);
        bodies.push_back(j);
    }
}

Vector3 GKinect::get_joint(int p_joint_id) 
{
    //ColorSpacePoint point;
    //mapper->MapCameraPointToColorSpace(joints[JointType_Head].Position, &point);
    //std::cout << "Head position: (" << point.X << ", " << point.Y << ")" << std::endl;

    //Godot::print(p_joint_id);

    if (p_joint_id >= JointType_Count || p_joint_id < 0)
    {
        return Vector3(0, 0, 0);
    }
    else
    {
        return Vector3(bodies[0][p_joint_id].Position.X, bodies[0][p_joint_id].Position.Y, bodies[0][p_joint_id].Position.Z);
    }
}

inline Vector3 JointToVec(Joint j)
{
    return Vector3(j.Position.X, j.Position.Y, j.Position.Z);
}

Dictionary GKinect::get_skeleton()
{
    Dictionary skeleton;
    int body_id = 0;
    for (int i = 0; i < JointType_Count; i++)
    {
        switch (i)
        {
        case JointType_SpineBase:
            skeleton["spine_base"] = JointToVec(bodies[body_id][i]);
            break;
        case JointType_SpineMid:
            skeleton["spine_mid"] = JointToVec(bodies[body_id][i]);
            break;
        case JointType_Neck:
            skeleton["neck"] = JointToVec(bodies[body_id][i]);
            break;
        case JointType_ShoulderLeft:
            skeleton["shoulder_left"] = JointToVec(bodies[body_id][i]);
            break;
        case JointType_ElbowLeft:
            skeleton["elbow_left"] = JointToVec(bodies[body_id][i]);
            break;
        case JointType_WristLeft:
            skeleton["wrist_left"] = JointToVec(bodies[body_id][i]);
            break;
        case JointType_HandLeft:
            skeleton["hand_left"] = JointToVec(bodies[body_id][i]);
            break;
        case JointType_ShoulderRight:
            skeleton["shoulder_right"] = JointToVec(bodies[body_id][i]);
            break;
        case JointType_ElbowRight:
            skeleton["elbow_right"] = JointToVec(bodies[body_id][i]);
            break;
        case JointType_WristRight:
            skeleton["wrist_right"] = JointToVec(bodies[body_id][i]);
            break;
        case JointType_HandRight:
            skeleton["hand_right"] = JointToVec(bodies[body_id][i]);
            break;
        case JointType_HipLeft:
            skeleton["hip_left"] = JointToVec(bodies[body_id][i]);
            break;
        case JointType_KneeLeft:
            skeleton["knee_left"] = JointToVec(bodies[body_id][i]);
            break;
        case JointType_AnkleLeft:
            skeleton["ankle_left"] = JointToVec(bodies[body_id][i]);
            break;
        case JointType_FootLeft:
            skeleton["foot_left"] = JointToVec(bodies[body_id][i]);
            break;
        case JointType_HipRight:
            skeleton["hip_right"] = JointToVec(bodies[body_id][i]);
            break;
        case JointType_Head:
            skeleton["head"] = JointToVec(bodies[body_id][i]);
            break;
        
        default:
            break;
        }
    }
    //skeleton["head"] = Vector3(bodies[0][JointType_Head].Position.X, bodies[0][JointType_Head].Position.Y, bodies[0][JointType_Head].Position.Z);
    return skeleton;
}

void GKinect::update_joints()
{
    IMultiSourceFrame* frame = NULL;
    if (SUCCEEDED(reader->AcquireLatestFrame(&frame)))
    {
        IBodyFrame* bodyframe;
        IBodyFrameReference* bodyframeref = NULL;
        frame->get_BodyFrameReference(&bodyframeref);
        bodyframeref->AcquireFrame(&bodyframe);
        if (bodyframeref) bodyframeref->Release();

        //jointList.clear();
        if (bodyframe)
        {
            IBody* body[BODY_COUNT] = { 0 };
            bodyframe->GetAndRefreshBodyData(BODY_COUNT, body);
            for (int i = 0; i < BODY_COUNT; i++) {
                body[i]->get_IsTracked(&tracked);
                if (tracked) {
                    body[i]->GetJoints(JointType_Count, &bodies[0][0]);
                    //jointList.push_back(joints);
                    break;
                }
            }

            //delete body;

            if (bodyframe) bodyframe->Release();
        }


        if (frame) {
            frame->Release();
        }


    }
}

void GKinect::_bind_methods() {
    ClassDB::bind_method(D_METHOD("update_joints"), &GKinect::update_joints);
    ClassDB::bind_method(D_METHOD("get_skeleton"), &GKinect::get_skeleton);
    ClassDB::bind_method(D_METHOD("initialize"), &GKinect::initialize);
    ClassDB::bind_method(D_METHOD("get_joint", "joint_id"), &GKinect::get_joint);
}