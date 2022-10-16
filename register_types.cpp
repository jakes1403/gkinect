#include "register_types.h"

#include "core/class_db.h"
#include "core/engine.h"
#include "gkinect.h"

static GKinect* GKinectPtr = NULL;

void register_gkinect_types() {
    ClassDB::register_class<GKinect>();

    GKinectPtr = memnew(GKinect);
    Engine::get_singleton()->add_singleton(Engine::Singleton("GKinect", GKinect::get_singleton()));

    #ifdef TOOLS_ENABLED
    EditorPlugins::add_by_type<GKinectEditorPlugin>();
    #endif
}

void unregister_gkinect_types() {
    memdelete(GKinectPtr);
}