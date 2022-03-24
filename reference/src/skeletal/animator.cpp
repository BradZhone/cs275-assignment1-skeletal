#include "skeletal/animator.hpp"

#include "gltf/tinygltf_helper.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

bool SkeletalAnimator::
loadFromTinyGLTF(
    const tinygltf::Model& mdl, 
    std::string& warn, 
    std::string& err
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
        int totalFrames = static_cast<int>(keyGetter.len);
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
                
                std::cout << "Animation Loaded for node " <<  channel.target_node << " at " << fTime 
                          << " with data " << x << " " << y << " " << z << " " << w << std::endl;

                /** These property might be helpful
                 * channel.target_node
                 * fTime
                 * quat: x, y, z, w
                 * */
                Keyframes temp = {channel.target_node, fTime, glm::quat(x,y,z,w)};
                this->keyframes.push_back(temp);
            }
        }
    }
    return true; 
}