/**
 * A design hint for you, modify anything you want
 * You can also choose to design in another way
 * */

#pragma once

#include <tiny_gltf.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>
#include <vector>

/***********************my code*****************************/
struct Keyframes {
    unsigned int target_nodes;
    float fTime;
    glm::quat orientation;
    
};
/***********************my code end*****************************/

class SkeletalAnimator
{
    ////////////
    // You might need a class to save keyframes 
    // generate animations by interpolation
    ////////////

/***********************my code*****************************/
private:
    std::vector<Keyframes> keyframes;

/***********************my code end*****************************/
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