/**
 * An example first person camera
 * Provide projection and view matrix to shader
 * 
 * You can modify whatever you want
 * */

#pragma once
#include <glm/glm.hpp>
#include <shader/shader.hpp>

class FirstPersonCamera
{
private:
    glm::vec3 cameraPos;
    glm::vec3 cameraForward;
    glm::vec3 cameraRight;
    glm::vec3 cameraGlobalUp;
    glm::vec3 cameraLocalUp;

    float fov;
    float aspect;
    float nearClipDist;
    float farClipDist;

public:
    FirstPersonCamera();
    void applyToShader(Shader *_shader);

    void applyProjection(Shader *_shader);
    void applyView(Shader *_shader);
    void applyCameraPos(Shader *_shader);
    
    void setAspect(float _aspect);
    void setCamera(float _fov, float _aspect, float _nearClipDist, float _farClipDist);
    
    void moveForward(float _dist);
    void moveBackward(float _dist);
    void moveLeft(float _dist);
    void moveRight(float _dist);
    void moveUp(float _dist);
    void moveDown(float _dist);

    void lookUp(float _angle);
    void lookDown(float _angle);
    void lookLeft(float _angle);
    void lookRight(float _angle);

    void rotateRight(float _angle);
    void rotateLeft(float _angle);

    void printInfo();
};