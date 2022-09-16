#ifndef GKINECT_H
#define GKINECT_H

#include "core/reference.h"
#include "core/math/vector3.h"
#include "core/dictionary.h"

class GKinect : public Reference {
    GDCLASS(GKinect, Reference);

protected:
    static void _bind_methods();
    static GKinect* singleton;

public:
    void initialize();
    void update_joints();
    Dictionary get_skeleton();
    Vector3 get_joint(int p_joint_id) ;

    static GKinect* get_singleton();

    GKinect();
};

#endif