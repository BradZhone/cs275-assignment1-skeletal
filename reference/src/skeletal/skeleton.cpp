#include "skeletal/skeleton.hpp"

#include <array>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "gltf/tinygltf_helper.h"

bool Skeleton::
loadFromTinyGLTF(
    const tinygltf::Model& mdl, 
    std::string& warn, 
    std::string& err
){
    if (mdl.skins.size() == 0) {
        err = "No skeleton data in file.";
        return false;
    }
    
    const tinygltf::Skin& skin = mdl.skins[0];

    if (skin.joints.size() == 0) {
        err = "No joints specified in skeleton.";
        return false;
    }

    // Find our root node.
    int root_id = skin.skeleton;
    if (root_id == -1) {
        // Some gltf files might not explicitly specify a root.
        // To make FK work, we need to make an educated guess.
        warn += "\nFile does not specify a root skeleton node. " \
                "Assuming first node index in `skin.joints` as root.";
        root_id = skin.joints[0];
    }
    /****************************************My Code ***************************************************/
    auto invBindGetter = tinygltf_buildDataGetter(mdl, skin.inverseBindMatrices);
    std::cout << "Num of Joints: " << skin.joints.size() << std::endl;
    /****************************************My Code end***************************************************/
    this->joints.resize(skin.joints.size());
    // this->joints.clear();

    for (size_t i = 0; i < skin.joints.size(); ++i) {
        //The current joint...
        int j_id = skin.joints[i];
        auto curNode = mdl.nodes[j_id];

        std::cout << "Skeleton Joints " << j_id << " Loaded: " << curNode.name << std::endl;

        /****************************************My Code ***************************************************/
        this->joints[j_id].name = curNode.name; // bound name to joint.name
        this->root = root_id; // bound root with j_id 
        /****************************************My Code end***************************************************/

        if (curNode.translation.size() == 0) {
            // Assign a default idenety translation
        } else if (curNode.translation.size() >= 3) {
            float x = static_cast<float>(curNode.translation[0]);
            float y = static_cast<float>(curNode.translation[1]);
            float z = static_cast<float>(curNode.translation[2]);
            
            /////////
            // Save into your own structure
            /////////
            /****************************************My Code ***************************************************/
            this->joints[j_id].basePosition = glm::vec3(x,y,z);
            /****************************************My Code end***************************************************/
            std::cout << "Position: " << x << " " << y << " " << z << std::endl;
        }

        if (curNode.rotation.size() == 0) {
            // Assign a default idenety translation
        } else if (curNode.rotation.size() >= 4) {
            // WARN: gltf: xyzw, eigen/glm: wxyz

            float x = static_cast<float>(curNode.rotation[3]);
            float y = static_cast<float>(curNode.rotation[0]);
            float z = static_cast<float>(curNode.rotation[1]);
            float w = static_cast<float>(curNode.rotation[2]);

            /////////
            // Save into your own structure
            /////////
            /****************************************My Code ***************************************************/
            this->joints[j_id].baseQuaternion = glm::quat(x,y,z,w);
            /****************************************My Code end***************************************************/
            std::cout << "Quaternion: " << x << " " << y << " " << z << " " << w << std::endl;
        }
        
        /**************************************My Code ***************************************************/
        if (curNode.scale.size() == 0) {
            // Assign a default idenety translation
        } else if (curNode.scale.size() >= 3) {
            // WARN: gltf: xyzw, eigen/glm: wxyz

            float x = static_cast<float>(curNode.scale[0]);
            float y = static_cast<float>(curNode.scale[1]);
            float z = static_cast<float>(curNode.scale[2]);

            this->joints[j_id].baseScale = glm::vec3(x,y,z);
            std::cout << "Scale: " << x << " " << y << " " << z << std::endl;
        }
        /****************************************My Code ebd***************************************************/

        // For some glTF files, the translations and rotations are saved using a single transform matrix curNode.matrix
        // however we ignore the format since it requires a matrix decomposition to get quaternion and translation seperatedly
        // if (curNode.matrix.size() == 16) {}
            
        // For some glTF files, there are inverseBindMatrix saved with each joints
        // we ignore it since we are always computing the inverseBindMatrix ourselves with initially defined joints
        /**
         * auto invBindGetter = tinygltf_buildDataGetter(mdl, skin.inverseBindMatrices);
         * if (invBindGetter.elementSize != 16 * sizeof(float)) {}
         * */
        
        // Access children of the joints
        /****************************************My Code***************************************************/
        this->joints[j_id].numChildren = curNode.children.size(); // write attJoint的numChildren属性

        std::cout << "Children: ";
        for (size_t j = 0; j < curNode.children.size(); ++j) {
            int child_id = curNode.children[j];
            std::cout << child_id << " ";
            auto childNode = mdl.nodes[child_id];
            this->joints[child_id].Parent = j_id; // bound the children joints' mparent attribute with current j_id index
            this->joints[j_id].Children.push_back(child_id); // push the children index into the tail of joints.Children vector
        }
        std::cout << std::endl << std::endl;

        // maybe calclating FK and inverseBindMatrix here..?
        // FK algorithm part1
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::scale(trans, this->joints[j_id].baseScale);

        trans = glm::mat4_cast(this->joints[j_id].baseQuaternion)*trans;
        glm::mat4 trans2 = glm::mat4(1.0f);
        trans2 = glm::translate(trans2, this->joints[j_id].basePosition);
        trans = trans2 * trans;
       
        if (j_id == root_id)
        {
            this->joints[j_id].invBindMatrix = trans;
        } else {
            this->joints[j_id].invBindMatrix = this->joints[this->joints[j_id].Parent].invBindMatrix * trans;
        }

        
    }

    // FK algorithm part2
    for (int i=0; i<this->joints.size();i++)
    {
        glm::vec4 temp = glm::vec4(this->joints[i].position,1.0f);
        temp = joints[i].invBindMatrix * temp;
        this->joints[i].position.x = temp.x;
        this->joints[i].position.y = temp.y;
        this->joints[i].position.z = temp.z;
        std::cout<<"Original position of joint "<<i<<":("<<temp.x<<","<<temp.y<<","<<temp.z<<")"<<std::endl;
    }

    /****************************************My Code end***************************************************/

    return true;
}
