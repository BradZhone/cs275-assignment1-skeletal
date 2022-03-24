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
#include <skeletal/skeleton.hpp>
/***********************my code end*****************************/


/***********************my code*****************************/
struct Keyframes {
    glm::quat orientation={0,0,0,1};
    glm::vec3 positions;
};

struct TimeTable {
    std::vector < float > ftime; // represent the time of each keyframe
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
    std::vector< std::vector< Keyframes > > keyframes; // using index to represent the node id.
    TimeTable timetable;
/***********************my code end*****************************/
public:
    bool loadFromTinyGLTF(
        const tinygltf::Model& mdl,
        std::string& warn,
        std::string& err,
        Skeleton* _skel // using skeleton.getjoints() to get the data of original data of joints.
    );
    const auto& getKeyframes() const { return keyframes; }
    const auto& getTimetable() const { return timetable; }
};