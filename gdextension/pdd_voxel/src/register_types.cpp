#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>

#include "pdd_voxel_node.h"

using namespace godot;

static void initialize_pdd_voxel(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }
    ClassDB::register_class<PDDVoxelNode>();
}

static void uninitialize_pdd_voxel(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }
}

extern "C"
{

    // This name MUST match `entry_symbol` in pdd_voxel.gdextension.
    GDExtensionBool GDE_EXPORT pdd_voxel_library_init(
        GDExtensionInterfaceGetProcAddress p_get_proc_address,
        GDExtensionClassLibraryPtr p_library,
        GDExtensionInitialization *r_initialization)
    {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
        init_obj.register_initializer(initialize_pdd_voxel);
        init_obj.register_terminator(uninitialize_pdd_voxel);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
        return init_obj.init();
    }

} // extern "C"
