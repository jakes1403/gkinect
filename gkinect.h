#ifndef GKINECT_H
#define GKINECT_H

#include "core/reference.h"
#include "core/math/vector3.h"
#include "core/dictionary.h"
#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "scene/2d/canvas_item.h"
#include "scene/gui/box_container.h"

class GKinect : public Reference {
    GDCLASS(GKinect, Reference);

protected:
    static void _bind_methods();
    static GKinect* singleton;

public:
    void initialize();
    void update_joints();
    Dictionary get_skeleton();
    Vector3 get_joint(int p_body_id, int p_joint_id);

    static GKinect* get_singleton();

    GKinect();
};

class GKinectEditor : public TextureRect {

	GDCLASS(GKinectEditor, TextureRect);

private:
	EditorNode *editor;

	static GKinectEditor *singleton;

public:

	static GKinectEditor *get_singleton() { return singleton; }

    void _process(float delta);

	GKinectEditor(EditorNode *p_editor);
};

class GKinectEditorPlugin : public EditorPlugin {

	GDCLASS(GKinectEditorPlugin, EditorPlugin);

    GKinectEditor* g_kinect_editor;

    EditorNode *editor;

public:
	virtual String get_name() const { return "Gesture Editor"; }
	bool has_main_screen() const { return true; }
	virtual void make_visible(bool p_visible);

    GKinectEditor *get_g_kinect_editor() { return g_kinect_editor; }

	GKinectEditorPlugin(EditorNode *p_node);
	~GKinectEditorPlugin();
};

#endif