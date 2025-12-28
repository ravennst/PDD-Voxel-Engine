#include "pdd_voxel_node.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void PDDVoxelNode::_bind_methods()
{
    // Nothing yet. This is intentionally minimal.
}

void PDDVoxelNode::_ready()
{
    UtilityFunctions::print("PDDVoxelNode loaded: GDExtension is working.");
}
