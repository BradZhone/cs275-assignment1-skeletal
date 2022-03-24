/***************************my code*************************/
#pragma once

#include <string>
#include <vector>

#include <tiny_gltf.h>
#include <eigen3/Eigen/Geometry>

#include "camera/fpc.hpp"
#include "shader/shader.hpp"
#include "skeletal/mesh.hpp"

#define MAX_BONE_INFLUENCE 4

class WireframeMeshPipeline
{
private:
    typedef struct VertexData {
        // position
        glm::vec3 positions;
        // normal
        glm::vec3 normals;
        // texCoords
        glm::vec2 uvs;
        //bone indexes which will influence this vertex
        glm::uvec4 influences;
        //weights from each bone
        glm::vec4 weights;
    } VertexData;

    struct GLO {
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
    } glo;

    std::vector<VertexData> vertices;
    std::vector<unsigned int> indices;

    Shader* shader;
    FirstPersonCamera* camera;

public:
    WireframeMeshPipeline(
        Shader* _shader, 
        FirstPersonCamera* _camera,
        BoneWeightedMesh* _mesh
    );

    void draw();
};

/***************************my code end*************************/