/**
 * A reference implementation of loading mesh and skinning weights from a gltf object
 * You can modify whatever you want or even use another library to load some other file formats
 * 
 * Check the gltf 2.0 specification if you feel confused
 * https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html
 * */
#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <tiny_gltf.h>

struct BoneWeightedMesh {
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;

    bool hasNormals;
    bool hasUVs;

    std::vector<glm::uvec4> influences;
    std::vector<glm::vec4> weights;

public:
    /** 
     * Some reference code to load data with tinygltf
     * Check the gltf 2.0 specification if you feel confused
     * https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html 
     * */
    bool loadFromTinyGLTF(
        const tinygltf::Model& mdl, 
        std::string& warn, 
        std::string& err
    );
};
