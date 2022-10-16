// Just some sample code from a tutorial
// This is a module I am creating for the Godot game engine
// so that it supports the microsoft kinect.
#include "gkinect.h"


#include <Windows.h>
#include <Ole2.h>

#include <Kinect.h>

#include <vector>

#include "core/os/os.h"
#include "core/image.h"

IKinectSensor* sensor = NULL;             // Kinect sensor
IMultiSourceFrameReader* reader = NULL;   // Kinect data source
ICoordinateMapper* mapper = NULL;

BOOLEAN tracked;

Joint bodies[BODY_COUNT][JointType_Count];

GKinect* GKinect::singleton;

static bool already_initialized = false;

static unsigned char* image;
size_t imageSize = 1920 * 1080 * 4;

GKinect::GKinect() {
    singleton = this;
}

GKinect* GKinect::get_singleton()
{
    return singleton;
}

void GKinect::initialize()
{
    if (!already_initialized)
    {
        if (FAILED(GetDefaultKinectSensor(&sensor)))
        {
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
            return;
        }
        already_initialized = true;
    }
}

inline Vector3 JointToVec(Joint j)
{
    return Vector3(j.Position.X, j.Position.Y, j.Position.Z);
}

Vector3 GKinect::get_joint(int p_body_id, int p_joint_id) 
{
    if (p_joint_id >= JointType_Count || p_joint_id < 0)
    {
        return Vector3(0, 0, 0);
    }
    else
    {
        return JointToVec(bodies[p_body_id][p_joint_id]);
    }
}

inline std::string JointToName(int j)
{
    switch (j)
    {
        case JointType_SpineBase:
            return "spine_base";
        case JointType_SpineMid:
            return "spine_mid";
        case JointType_Neck:
            return "neck";
        case JointType_Head:
            return "head";
        case JointType_ShoulderLeft:
            return "shoulder_left";
        case JointType_ElbowLeft:
            return "elbow_left";
        case JointType_WristLeft:
            return "wrist_left";
        case JointType_HandLeft:
            return "hand_left";
        case JointType_ShoulderRight:
            return "shoulder_right";
        case JointType_ElbowRight:
            return "elbow_right";
        case JointType_WristRight:
            return "wrist_right";
        case JointType_HandRight:
            return "hand_right";
        case JointType_HipLeft:
            return "hip_left";
        case JointType_KneeLeft:
            return "knee_left";
        case JointType_AnkleLeft:
            return "ankle_left";
        case JointType_FootLeft:
            return "foot_left";
        case JointType_HipRight:
            return "hip_right";
        case JointType_KneeRight:
            return "knee_right";
        case JointType_AnkleRight:
            return "ankle_right";
        case JointType_FootRight:
            return "foot_right";
        case JointType_SpineShoulder:
            return "spine_shoulder";
        case JointType_HandTipLeft:
            return "hand_tip_left";
        case JointType_ThumbLeft:
            return "thumb_left";
        case JointType_HandTipRight:
            return "hand_tip_right";
        case JointType_ThumbRight:
            return "thumb_right";
        default:
            return "null";
    }
}

Dictionary GKinect::get_skeleton()
{
    Dictionary skeleton;
    int body_id = 0;
    for (int i = 0; i < JointType_Count; i++)
    {
        const char* name = JointToName(i).c_str();
        skeleton[name] = JointToVec(bodies[body_id][i]);
    }
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

        if (bodyframe)
        {
            IBody* body[BODY_COUNT] = { 0 };
            bodyframe->GetAndRefreshBodyData(BODY_COUNT, body);
            int tracked_bodies = 0;
            for (int i = 0; i < BODY_COUNT; i++)
            {
                body[i]->get_IsTracked(&tracked);
                if (tracked)
                {
                    tracked_bodies += 1;
                    body[i]->GetJoints(JointType_Count, &bodies[i][0]);
                }
            }

            if (bodyframe) bodyframe->Release();
        }


        if (frame)
        {
            frame->Release();
        }


    }
}

void GKinect::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("update_joints"), &GKinect::update_joints);
    ClassDB::bind_method(D_METHOD("get_skeleton"), &GKinect::get_skeleton);
    ClassDB::bind_method(D_METHOD("initialize"), &GKinect::initialize);
    ClassDB::bind_method(D_METHOD("get_joint", "body_id", "joint_id"), &GKinect::get_joint);
}

GKinectEditor *GKinectEditor::singleton = NULL;

GKinectEditor::GKinectEditor(EditorNode *p_editor)
{
    editor = p_editor;
    singleton = this;
    OS::get_singleton()->print("Hello!");
}

void GKinectEditorPlugin::make_visible(bool p_visible)
{
    if (p_visible)
    {
        g_kinect_editor->show();
    }
    else
    {
        g_kinect_editor->hide();
    }
}

GKinectEditorPlugin::GKinectEditorPlugin(EditorNode *p_node)
{
    editor = p_node;
    g_kinect_editor = memnew(GKinectEditor(editor));
    editor->get_viewport()->add_child(g_kinect_editor);
    g_kinect_editor->hide();

    GKinect* g_singleton = GKinect::get_singleton();
    g_singleton->initialize();

    memalloc(imageSize);
    image = (unsigned char*)malloc(imageSize);
    memset(image, 255, imageSize);

    Image a;
    a.create(1920, 1080, true, Image::FORMAT_RGBA8, image);
}


GKinectEditorPlugin::~GKinectEditorPlugin()
{
}