cmake_minimum_required(VERSION 3.20)
project(pdd_voxel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build your extension as a shared library (DLL on Windows).
add_library(pdd_voxel SHARED
    src/register_types.cpp
    src/pdd_voxel_node.cpp
)

# Bring in godot-cpp (as a submodule) and link it.
# This assumes your layout:
#   gdextension/godot-cpp/
#   gdextension/pdd_voxel/
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../godot-cpp
                 ${CMAKE_CURRENT_BINARY_DIR}/godot-cpp-build)

# godot-cpp's exported target name can differ by version.
# Try the modern names first; if one fails, switch to the other.
target_link_libraries(pdd_voxel PRIVATE godot-cpp)

target_include_directories(pdd_voxel PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Put the built DLL exactly where the .gdextension file expects it.
# We match the names in pdd_voxel.gdextension:
#   pdd_voxel.windows.debug.x86_64.dll
#   pdd_voxel.windows.release.x86_64.dll
set_target_properties(pdd_voxel PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/windows"
)

# Name the output with config + arch so Godot feature flags can select it.
if(MSVC)
    # Multi-config generator (Visual Studio). CMAKE_BUILD_TYPE is usually empty.
    set_target_properties(pdd_voxel PROPERTIES
        OUTPUT_NAME "pdd_voxel.windows.$<IF:$<CONFIG:Debug>,debug,release>.x86_64"
    )
else()
    # If you ever use Ninja/MinGW etc.
    set_target_properties(pdd_voxel PROPERTIES
        OUTPUT_NAME "pdd_voxel.windows.$<IF:$<CONFIG:Debug>,debug,release>.x86_64"
    )
endif()
