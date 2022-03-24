/**
 * A design hint for you, modify whatever you want
 * You can also choose to design in another way
 * */
#pragma once

#include <vector>
#include <unordered_map>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>
#include <tiny_gltf.h>

#include "shader/shader.hpp"

#define MAX_INFLUENCE_BONE_NUM 4

struct Joint {
    ///////////
    // Some properties to save..?
    ///////////
    /****************************************My Code***************************************************/
    std::string name;
    glm::vec3 basePosition;
    glm::quat baseQuaternion;
    glm::vec3 baseScale = {1.0f, 1.0f, 1.0f};
    glm::mat4 invBindMatrix = glm::mat4(1.0f);
    glm::vec3 position = {0,0,0};

    std::vector<int> Children;
    int Parent = -1;
    int numChildren;
    
    /****************************************My Code end***************************************************/
};

class Skeleton {
private:
    // Joint* root;
    int root; //using index num to represent root.
    std::vector<Joint> joints;

public:
    size_t getBoneNum() const { return this->joints.size(); }
    const auto& getJoints() const { return joints; }
    const auto& getRoot() const { return root; }

    /** 
     * Some reference code to load data with tinygltf
     * Check the gltf 2.0 specification if you feel confused
     * https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html 
     * */
    bool loadFromTinyGLTF(const tinygltf::Model& mdl, std::string& warn, std::string& err);
};