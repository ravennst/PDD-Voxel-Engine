#pragma once

#include <godot_cpp/classes/node.hpp>

namespace godot
{

    class PDDVoxelNode : public Node
    {
        GDCLASS(PDDVoxelNode, Node);

    protected:
        static void _bind_methods();

    public:
        PDDVoxelNode() = default;
        ~PDDVoxelNode() override = default;

        void _ready() override;
    };

} // namespace godot
