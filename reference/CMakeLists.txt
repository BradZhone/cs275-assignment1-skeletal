####################
# A simple CMake Project File for your reference
# Modify whatever you want
# You can also use other project frameworks
####################

cmake_minimum_required(VERSION 3.18)

project(skeletal-animation)
set (CMAKE_CXX_STANDARD 20)
set(CMAKE_TOOLCHAIN_FILE "C:\\dev\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake")
# It is recommended to use vcpkg to install these package
# visit https://github.com/Microsoft/vcpkg for more information
find_package(Eigen3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED) # dependencies of tinygltf
find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")

add_library(libmain 
    src/camera/fpc.cpp
    src/shader/shader.cpp
    src/gltf/tinygltf_helper.cpp
    src/skeletal/skeleton.cpp
    src/skeletal/animator.cpp
    src/skeletal/mesh.cpp
    src/pipeline/skeleton.cpp
    src/pipeline/mesh.cpp
)
target_include_directories(libmain
    PUBLIC ${TINYGLTF_INCLUDE_DIRS}
    PUBLIC include
)
add_library(libdeps INTERFACE)
target_link_libraries(libdeps INTERFACE
    Eigen3::Eigen
    glfw
    glm::glm
    glad::glad
    nlohmann_json::nlohmann_json
    libmain
)

# The main opengl framework to of rendering
add_executable(main test/main.cpp)
add_dependencies(main libmain)
target_link_libraries(main PRIVATE libdeps)

# Some example to use tinygltf
add_executable(tinygltfExamples test/tinygltfExamples.cpp)
add_dependencies(tinygltfExamples libmain)
target_link_libraries(tinygltfExamples libdeps)