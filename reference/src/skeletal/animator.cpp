/***********************my code*****************************/

#include "skeletal/animator.hpp"
#include "gltf/tinygltf_helper.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <skeletal/skeleton.hpp>
/***********************my code end*****************************/

bool SkeletalAnimator::
loadFromTinyGLTF(
    const tinygltf::Model& mdl, 
    std::string& warn, 
    std::string& err,
    Skeleton* _skel
) {
    if (mdl.animations.size() == 0) {
        err = "No skeletal animation data in file.";
        return false;
    }

    const tinygltf::Animation& gltfAnim = mdl.animations[0];

    //TimeGetter will grab the timestamp of a given keyframe.
    //KeyGetter will grab the actual keyframe data.
    tinygltf_DataGetter timeGetter, keyGetter;

    float maxTime = 0.0f; 

    /***********************my code*****************************/
    auto joints = _skel->getJoints();
    int root = _skel->getRoot();
    this->keyframes.resize(joints.size()); // let the keyframes' size as large as the number of nodes.
    /***********************my code end*****************************/

    for (size_t i = 0; i < gltfAnim.channels.size(); ++i)
    {
        const tinygltf::AnimationChannel& channel = gltfAnim.channels[i];
        const tinygltf::AnimationSampler& sampler = gltfAnim.samplers[channel.sampler];

        // `channel.target_node` tells you the index of nodes in gltf file is using the animation

        timeGetter = tinygltf_buildDataGetter(mdl, sampler.input);
        keyGetter = tinygltf_buildDataGetter(mdl, sampler.output);

        if (timeGetter.len == 0 || keyGetter.len == 0)
            continue;
        
        if (timeGetter.elementSize != sizeof(float)) {
            err = "The time of the animation per keyframe should be a float number";
            return false;
        }
        if (keyGetter.elementSize != 4 * sizeof(float)) {
            err = "The data of the animation per keyframe should be a quaternion consists of 4 floats";
            return false;
        }
        
        // Normally we'd use templates to keep this sort of thing cleaned up.
        // Unfortunately, that whole difference in quaternion layout throws a
        // wrinkle into that - we can't just memcpy into a raw glm::quat.
        // Rather than correct after the fact and swap X/W coordinates,
        // we'll just do this.
        
        // We only care about joint rotation.
        if (channel.target_path == "rotation") {
            for (size_t k = 0; k < timeGetter.len && k < keyGetter.len; ++k) {
                float fTime;
                float x, y, z, w;

                memcpy(&fTime, &timeGetter.data[k * timeGetter.stride], sizeof(float));

                if (fTime > maxTime)
                    maxTime = fTime;

                memcpy(&x, &keyGetter.data[k * keyGetter.stride + 0 * sizeof(float)], sizeof(float));
                memcpy(&y, &keyGetter.data[k * keyGetter.stride + 1 * sizeof(float)], sizeof(float));
                memcpy(&z, &keyGetter.data[k * keyGetter.stride + 2 * sizeof(float)], sizeof(float));
                memcpy(&w, &keyGetter.data[k * keyGetter.stride + 3 * sizeof(float)], sizeof(float));
                
                // std::cout << "Animation Loaded for node " <<  channel.target_node << " at " << fTime 
                //           << " with data " << x << " " << y << " " << z << " " << w << std::endl;
                
                /** These property might be helpful
                 * channel.target_node
                 * fTime
                 * quat: x, y, z, w
                 * */
                /***********************my code*****************************/
                Keyframes temp = {glm::quat(w,x,y,z), glm::vec3(0,0,0)};
                this->keyframes[channel.target_node].push_back(temp);
                //add the timestamp into the time structure.
                if (i==0) // using a flag to store time data once.
                    this->timetable.ftime.push_back(fTime);
                /***********************my code end*****************************/
            }
        // } else{
        //     for (size_t k = 0; k < timeGetter.len && k < keyGetter.len; ++k) {
        //         Keyframes temp = {glm::quat(0,0,0,1), glm::vec3(0,0,0)};
        //         this->keyframes[i].push_back(temp);
        //     }
        }
    }
    /***********************my code*****************************/
    const auto jointInvBindMatrix = joints;
    for(int f=0;f<this->timetable.ftime.size();f++)
    {
        // for(int j_id=joints.size()-1;j_id>=0;j_id--)
        for(int j_id=0; j_id<joints.size();j_id++)
        {
            // glm::mat4 trans = glm::mat4(1.0f);
            // trans = glm::scale(trans, joints[j_id].baseScale);
            // trans = glm::mat4_cast(this->keyframes[j_id][f].orientation)*trans;
            // // trans = glm::mat4_cast(joints[j_id].baseQuaternion)*trans;
            // glm::mat4 trans2 = glm::mat4(1.0f);
            // trans2 = glm::translate(trans2, joints[j_id].basePosition);
            // trans = trans2 * trans;
        
            // if (j_id == root)
            // {
            //     joints[j_id].invBindMatrix = trans;
            // } else {
            //     joints[j_id].invBindMatrix = joints[joints[j_id].Parent].invBindMatrix * trans;
            // }

            joints[j_id].invBindMatrix = glm::mat4_cast(this->keyframes[j_id][f].orientation) * jointInvBindMatrix[j_id].invBindMatrix;

        }

        for(int j_id=0;j_id<joints.size();j_id++)
        {
            glm::vec4 temp = glm::vec4(0,0,0,1.0f);
            temp = joints[j_id].invBindMatrix * temp;
            this->keyframes[j_id][f].positions.x = temp.x;
            this->keyframes[j_id][f].positions.y = temp.y;
            this->keyframes[j_id][f].positions.z = temp.z;
            // std::cout<<"Original position of joint "<<i<<":("<<temp.x<<","<<temp.y<<","<<temp.z<<")"<<std::endl;
        }
    }
    /***********************my code end*****************************/
    return true; 
}