/**
 * A partial implmentation to show how to draw with opengl
 * this pipeline is intended to draw a skeleton with lines
 * but not finished since it is combined with interface of 
 * the class Skeleton
 * 
 * You can modify what ever you want or even delete and take
 * another brand new design on your own
 * */

#pragma once

#include <tiny_gltf.h>
#include <eigen3/Eigen/Geometry>

#include "camera/fpc.hpp"
#include "shader/shader.hpp"
#include "skeletal/skeleton.hpp"

class WireframeSkeletonPipeline
{
private:
    typedef struct VertexData { // skeleton.vs
        glm::vec3 position = {0,0,0}; /***********************************My Code ***************************************************/
    } VertexData;

    struct GLO {
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
    } glo;

    std::vector<unsigned int> indices;
    std::vector<VertexData> vertices;

    Shader* shader;
    FirstPersonCamera* camera;
    Skeleton* skeleton;

public:
    WireframeSkeletonPipeline(
        Shader* _shader, 
        FirstPersonCamera* _camera,
        Skeleton* _skel
    );

    void draw();

private:
    void setupIndices();
    void updateVertices();
};