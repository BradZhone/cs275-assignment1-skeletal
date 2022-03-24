/**
 * A design hint for you, modify anything you want
 * You can also choose to design in another way
 * */

#pragma once

#include <tiny_gltf.h>

class SkeletalAnimator
{
    ////////////
    // You might need a class to save keyframes 
    // generate animations by interpolation
    ////////////

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