
#include "gltf/tinygltf_helper.h"

#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "skeletal/mesh.hpp"
#include "skeletal/animator.hpp"
#include "skeletal/skeleton.hpp"

// A simple mesh structure to show how to use tinygltf to load data
struct ExampleMesh {
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    
    bool hasNormals;
    bool hasUVs;
};

bool processPrimitive(
    const tinygltf::Model& gltf, 
    size_t geomIndex,
    bool flipUVY,
    std::vector<unsigned int>& indices,
    std::vector<glm::vec3>& verts, 
    std::vector<glm::vec2>& uvs,
    std::vector<glm::vec3>& normals, 
    bool& hasNormals, 
    bool& hasUVs,
    std::string& err, 
    std::string& warn
) {
    const tinygltf::Primitive geom = gltf.meshes[0].primitives[geomIndex];

    if (geom.indices == -1) {
        err = "File is missing primitive indices. "\
              "Consider changing your GLTF export settings, or else this loader " \
              "must be augmented to support files without indices.";
        return false;
    }

    // glTF stores data per-vertex.
    // This indexer will allow us to access the data that tells
    // us which vertices make up the faces of the object.
    // This allows us to specify our data in the order we need it
    // for OpenGL vertex buffers - in other words, spelling out the vertex
    // data as a set of triangles.
    auto faceIndexer = tinygltf_buildDataGetter(gltf, geom.indices);
    if (faceIndexer.elementSize != sizeof(GLshort)) {
        err = "Primitive indices are in a currently unsupported format. " \
              "Consider changing your GLTF export settings, or else this loader " \
              "must be augmented to support the provided format.";
        return false;
    }

    int vID = tinygltf_findAccessor(geom, "POSITION");
    if (vID == -1) {
        err = "No vertex positions found in mesh primitive " + std::to_string(geomIndex);
        return false;
    }
    int nID = tinygltf_findAccessor(geom, "NORMAL");
    hasNormals = nID != -1;
    if (!hasNormals) {
        warn += "\nNo normals found in mesh primitive " + std::to_string(geomIndex);
    }
    int uvID = tinygltf_findAccessor(geom, "TEXCOORD_0");
    hasUVs = uvID != -1;
    if (!hasUVs) {
        warn += "\nNo UVs found in mesh primitive " + std::to_string(geomIndex);
    }

    tinygltf_DataGetter vGetter, nGetter, uvGetter;

    vGetter = tinygltf_buildDataGetter(gltf, vID);
    if (vGetter.elementSize != sizeof(glm::vec3)) {
        err = "Vertex position data is in a currently unsupported format. " \
            "Consider changing your GLTF export settings, or else this loader " \
            "must be augmented to support the provided format.";
        return false;
    }
    if (hasNormals) {
        nGetter = tinygltf_buildDataGetter(gltf, nID);
        if (nGetter.elementSize != sizeof(glm::vec3)) {
            hasNormals = false;
            warn += "\nNormal data is in a currently unsupported format. " \
                "Consider changing your GLTF export settings, or else this loader " \
                "must be augmented to support the provided format.";
        }
    }
    if (hasUVs) {
        uvGetter = tinygltf_buildDataGetter(gltf, uvID);
        if (uvGetter.elementSize != sizeof(glm::vec2)) {
            hasUVs = false;
            warn += "\nUV data is in a currently unsupported format. " \
                "Consider changing your GLTF export settings, or else this loader " \
                "must be augmented to support the provided format.";
        }
    }

    size_t startIndex = verts.size();
    size_t indiceOffset = indices.size();
    
    indices.resize(indiceOffset + faceIndexer.len);
    std::cout << "Loading Mesh with " << faceIndexer.len << " indices." << std::endl;
    
    verts.resize(startIndex + vGetter.len);
    std::cout << "Loading Mesh with " << vGetter.len << " vertices." << std::endl;

    if (hasNormals) {
        normals.resize(startIndex + nGetter.len);
    }
    if (hasUVs) {
        uvs.resize(startIndex + uvGetter.len);
    }

    // This is the bit where we actually get to extracting our data.
    for (size_t i = 0; i < faceIndexer.len; ++i)
    {
        // What vertex do we need to look at?
        GLshort vertIndex;
        memcpy(&vertIndex, &faceIndexer.data[i * faceIndexer.stride], sizeof(GLshort));
        indices[indiceOffset + i] = startIndex + vertIndex;
    }

    for (size_t i = 0; i < vGetter.len; ++i) {
        memcpy(&verts[startIndex + i], &vGetter.data[i * vGetter.stride], sizeof(glm::vec3)); // grab vertex position.
    }
    if (hasNormals) {
        for (size_t i = 0; i < nGetter.len; ++i) {
            memcpy(&normals[startIndex + i], &nGetter.data[i * nGetter.stride], sizeof(glm::vec3)); // grab vertex normal.
        }
    }    
    if (hasUVs) {
        for (size_t i = 0; i < uvGetter.len; ++i) {
            memcpy(&uvs[startIndex + i], &uvGetter.data[i * uvGetter.stride], sizeof(glm::vec2)); //grab texture coordinates.
            // We may need to flip our vertical UV-coordinate.
            // You will probably need to do this, depending on your export settings/texture.
            if (flipUVY) {
                uvs[startIndex + i].y = 1.0f - uvs[startIndex + i].y;
            }
        }
    }

    return true;
}


bool ExampleExtractGeometry(
    const tinygltf::Model& gltf, 
    ExampleMesh& mesh,
    std::string& warn,
    std::string& err
) {
    
    bool flipUVY = false;
    
    if (gltf.meshes.size() == 0) {
        err = "No meshes in file.";
        return false;
    }
    
    const tinygltf::Mesh& meshData = gltf.meshes[0];
    if (meshData.primitives.size() == 0) {
        err = "No geometry data associated with mesh.";
        return false;
    }

    for (size_t i = 0; i < meshData.primitives.size(); ++i) {
        if (!processPrimitive(gltf, i, flipUVY, mesh.indices, mesh.verts, mesh.uvs, mesh.normals, mesh.hasNormals, mesh.hasUVs, err, warn))
            return false;
    }
    return true;
}

// A simple joint structure to show how to read skeleton structure using tinygltf
struct ExampleJoint {
    glm::vec3 basePosition;
    glm::quat baseQuaternion;
    glm::mat4 invBindMatrix;
};

bool ExampleExtractSkeleton(
    const tinygltf::Model& gltf,
    std::string& warn, 
    std::string& err
) {
    if (gltf.skins.size() == 0) {
        err = "No skeleton data in file.";
        return false;
    }

    const tinygltf::Skin& skin = gltf.skins[0];

    if (skin.joints.size() == 0) {
        err = "No joints specified in skeleton.";
        return false;
    }

    auto invBindGetter = tinygltf_buildDataGetter(gltf, skin.inverseBindMatrices);
    if (invBindGetter.elementSize != 16 * sizeof(float)) {
        err = "Inverse bind matrices are in a currently unsupported format. " \
              "Consider changing your GLTF export settings, or else check for " \
              "and support this format in your GLTF loader implementation.";
        return false;
    }

    // Find our root node.
    int root_id = skin.skeleton;
    if (root_id == -1) {
        // Some converters might not explicitly specify a root.
        // To make FK work, we need to make an educated guess.
        warn += "\nFile does not specify a root skeleton node. " \
                "Assuming minimum joint index as root.";
        root_id = skin.joints[0];
    }

    glm::mat4 jointMat = glm::mat4();
    glm::vec3 jointScale;
    glm::vec3 jointSkew;
    glm::vec4 jointPersp;
    
    // grab all the joint transform data for our skeleton.
    std::cout << "Num of Joints: " << skin.joints.size() << std::endl;
    for (size_t i = 0; i < skin.joints.size(); ++i) {
        //The current joint...
        int j_id = skin.joints[i];
        
        //The node representing our joint in the glTF hierarchy...
        const tinygltf::Node& node = gltf.nodes[skin.joints[i]];
        
        //If we're the root, set ourselves up as the root.
        if (!node.name.empty()) {
            if (j_id == root_id) {
                std::cout << "Root Node: " << j_id << " " << node.name << std::endl;
            } else {
                std::cout << j_id << " " << node.name << std::endl;
            }
        } else {
            if (j_id == root_id) {
                std::cout << "Root Node: " << j_id << node.name << std::endl;
            } else {
                std::cout << j_id << " " << node.name << std::endl;
            }
        }
        
        ExampleJoint joint; // copy node property to our defined joint

        // if the glTF file specifies position/rotation separately...
        if (node.translation.size() >= 3 && node.rotation.size() >= 4) {
            joint.basePosition = glm::vec3(
                static_cast<float>(node.translation[0]),
                static_cast<float>(node.translation[1]),
                static_cast<float>(node.translation[2])
            );
            
            // VERY IMPORTANT: glTF will specify quaternions in XYZW order.
            // GLM specifies quaternions in WXYZ order.
            // Any time a quaternion is "translated" into GLM, make sure to 
            // switch the order of components accordingly.
            joint.baseQuaternion = glm::quat(
                static_cast<float>(node.rotation[3]),
                static_cast<float>(node.rotation[0]),
                static_cast<float>(node.rotation[1]),
                static_cast<float>(node.rotation[2])
            );

        } else if (node.matrix.size() == 16) {
            for (int i = 0; i < 4; ++i)
            {
                jointMat[i] = glm::vec4(
                    static_cast<float>(node.matrix[i*4]),
                    static_cast<float>(node.matrix[i*4+1]),
                    static_cast<float>(node.matrix[i*4+2]),
                    static_cast<float>(node.matrix[i*4+3])
                );
            }

            //We're just using glm::decompose here since it's built-in with GLM
            //and straightforward (we could write our own function
            //to grab rotation and position alone, since that's all
            //we need, if we make some assumptions).
            glm::decompose(jointMat, jointScale,
                            joint.baseQuaternion, joint.basePosition,
                            jointSkew, jointPersp);
        } else {
            err = "Joint transforms are in a currently unsupported format." \
                    "Consider changing your GLTF export settings, or else check for " \
                    "and support this format in your GLTF loader implementation.";
            
            return false;
        }
        
        // copy the inverseBindMatrix instead of computing ourselves
        memcpy(&joint.invBindMatrix, &invBindGetter.data[i * invBindGetter.stride], 16 * sizeof(GLfloat)); 
    }

    // set up the parent-child relationship between all our joints.
    for (size_t i = 0; i < skin.joints.size(); ++i) {
        int j_id = skin.joints[i];

        // which node represents our joint in the glTF file...
        const tinygltf::Node& node = gltf.nodes[j_id];

        // node.children contains glTF IDs for the joints
        // that should be "under" our current joint in the hierarchy.
        for (size_t j = 0; j < node.children.size(); ++j) {
            int child_id = node.children[j];
            const tinygltf::Node& childNode = gltf.nodes[child_id];
            std::cout << node.name << "->" << childNode.name << std::endl;
        }
    }
    return true;
}

struct ExampleRotationFrame {
    float timestamp;
    glm::quat rotationData;
};

bool ExampleExtractSkeletalAnimation(const tinygltf::Model& gltf, std::string& err, std::string& warn)
{
    if (gltf.animations.size() == 0) {
        err = "No skeletal animation data in file.";
        return false;
    }

    const tinygltf::Animation& gltfAnim = gltf.animations[0];

    //TimeGetter will grab the timestamp of a given keyframe.
    //KeyGetter will grab the actual keyframe data.
    tinygltf_DataGetter timeGetter, keyGetter;

    float maxTime = 0.0f; 

    for (size_t i = 0; i < gltfAnim.channels.size(); ++i)
    {
        const tinygltf::AnimationChannel& channel = gltfAnim.channels[i];

        // (int) channel.target_node 
        const tinygltf::AnimationSampler& sampler = gltfAnim.samplers[channel.sampler];

        timeGetter = tinygltf_buildDataGetter(gltf, sampler.input);
        keyGetter = tinygltf_buildDataGetter(gltf, sampler.output);

        if (keyGetter.len == 0 || timeGetter.len == 0)
            continue;

        // Normally we'd use templates to keep this sort of thing cleaned up.
        // Unfortunately, that whole difference in quaternion layout throws a
        // wrinkle into that - we can't just memcpy into a raw glm::quat.
        // Rather than correct after the fact and swap X/W coordinates,
        // we'll just do this.

        //We only care about joint position/rotation.
        int totalFrames = static_cast<int>(keyGetter.len);
        if (channel.target_path == "rotation") {
            for (size_t k = 0; k < timeGetter.len && k < keyGetter.len; ++k) {
                float fTime;
                float x, y, z, w;
                glm::quat rot;

                memcpy(&fTime, &timeGetter.data[k * timeGetter.stride], sizeof(float));

                if (fTime > maxTime)
                    maxTime = fTime;

                memcpy(&x, &keyGetter.data[k * keyGetter.stride + 0 * sizeof(float)], sizeof(float));
                memcpy(&y, &keyGetter.data[k * keyGetter.stride + 1 * sizeof(float)], sizeof(float));
                memcpy(&z, &keyGetter.data[k * keyGetter.stride + 2 * sizeof(float)], sizeof(float));
                memcpy(&w, &keyGetter.data[k * keyGetter.stride + 3 * sizeof(float)], sizeof(float));

                rot = glm::normalize(glm::quat(w, x, y, z));
            }
        } else if (channel.target_path == "translation") {
            // we ignore the translation channel
            // since we are using rotation and FK to calculate translation ourselves
            for (size_t k = 0; k < timeGetter.len && k < keyGetter.len; ++k) {
                float fTime;
                glm::vec3 pos;

                memcpy(&fTime, &timeGetter.data[k * timeGetter.stride], sizeof(float));

                if (fTime > maxTime)
                    maxTime = fTime;

                memcpy(&pos, &keyGetter.data[k * keyGetter.stride], sizeof(glm::vec3));
            }
        }
    }
    return true;
}


bool ExampleExtractSkinWeights(const tinygltf::Model& gltf, std::string& err, std::string& warn) {
    if (gltf.meshes.size() == 0 || gltf.meshes[0].primitives.size() == 0) {
        err = "Cannot load joint influences and weights - no geometry data in file.";
        return false;
    }

    const tinygltf::Primitive& geom = gltf.meshes[0].primitives[0];

    int influenceID = tinygltf_findAccessor(geom, "JOINTS_0");
    int weightID = tinygltf_findAccessor(geom, "WEIGHTS_0");

    if (influenceID == -1) {
        err = "No joint influence data present in file.";
        return false;
    }
    if (weightID == -1) {
        err = "No skin weight data present in file.";
        return false;
    }

    auto infGetter = tinygltf_buildDataGetter(gltf, influenceID);
    auto wtGetter = tinygltf_buildDataGetter(gltf, weightID);
    auto faceIndexer = tinygltf_buildDataGetter(gltf, geom.indices);

    if (faceIndexer.elementSize != sizeof(GLshort)) {
        err = "Primitive indices are in a currently unsupported format." \
            "Consider changing your GLTF export settings, or else check for " \
            "and support this format in your GLTF loader implementation.";
        return false;
    }
    if (infGetter.elementSize != 4 * sizeof(GLubyte)) {
        err = "Joint influences are in a currently unsupported format." \
              "Consider changing your GLTF export settings, or else check for " \
              "and support this format in your GLTF loader implementation.";

        return false;
    }
    if (wtGetter.elementSize != 4 * sizeof(GLfloat)) {
        err = "Skin weights are in a currently unsupported format." \
              "Consider changing your GLTF export settings, or else check for " \
              "and support this format in your GLTF loader implementation.";

        return false;
    }

    std::vector<glm::ivec4> influences;
    std::vector<glm::vec4> weights;

    influences.resize(faceIndexer.len);
    weights.resize(faceIndexer.len);

    for (size_t i = 0; i < faceIndexer.len; ++i)
    {
        GLubyte j0, j1, j2, j3;

        GLshort faceIndex;
        memcpy(&faceIndex, &faceIndexer.data[i * faceIndexer.stride], sizeof(GLshort));

        size_t face = faceIndex;

        memcpy(&j0, &infGetter.data[face * infGetter.stride + 0 * sizeof(GLubyte)], sizeof(GLubyte));
        memcpy(&j1, &infGetter.data[face * infGetter.stride + 1 * sizeof(GLubyte)], sizeof(GLubyte));
        memcpy(&j2, &infGetter.data[face * infGetter.stride + 2 * sizeof(GLubyte)], sizeof(GLubyte));
        memcpy(&j3, &infGetter.data[face * infGetter.stride + 3 * sizeof(GLubyte)], sizeof(GLubyte));
    
        // Note: glTF appears to "switch" between how joints are indexed
        // when it comes to skin weights - the joint IDs will match
        // the ORDER of nodes in the skeleton definition, rather than the
        // IDENTIFIERS of those nodes.
        influences[i] = glm::ivec4(j0, j1, j2, j3);
        memcpy(&weights[i], &wtGetter.data[face * wtGetter.stride], sizeof(glm::vec4));
    }

    // influences
    // weights

    return true;
}

int main() {
    tinygltf::Model mdl;
    if (!tinygltf_parsefile("E:\\Projects\\OpenGL\\assignment1-skeletal-BradZhone-main\\reference\\res\\mdl\\dancing_cylinder.gltf", mdl)) {
    // if (!tinygltf_parsefile("E:\\Projects\\OpenGL\\assignment1-skeletal-BradZhone-main\\reference\\res\\mdl\\weaving_flag.gltf", mdl)) {
        return -1;
    }
    
    std::string warn, err;

    
    BoneWeightedMesh mesh;
    Skeleton skel;
    SkeletalAnimator anim;

    if (!mesh.loadFromTinyGLTF(mdl, warn, err)) {
        std::cout << "MeshLoaderError: " << err << std::endl;
    };
    if (!warn.empty()) {
        std::cout << "MeshLoaderWarning: " << warn << std::endl;
        warn.clear();
    }
    
    if (!skel.loadFromTinyGLTF(mdl, warn, err)) {
        std::cout << "SkeletonLoaderError: " << err << std::endl;
    };
    if (!warn.empty()) {
        std::cout << "SkeletonLoaderWarning: " << warn << std::endl;
        warn.clear();
    }

    if (!anim.loadFromTinyGLTF(mdl, warn, err)) {
        std::cout << "AnimationLoaderError: " << err << std::endl;
    };
    if (!warn.empty()) {
        std::cout << "AnimationLoaderWarning: " << warn << std::endl;
        warn.clear();
    }

    ExampleMesh example_mesh;
    if (!ExampleExtractGeometry(mdl, example_mesh, warn, err)) {
        std::cout << "GeometryLoaderError: " << err << std::endl;
    };
    if (!warn.empty()) {
        std::cout << "GeometryLoaderWarning: " << warn << std::endl;
        warn.clear();
    }

    if (!ExampleExtractSkeleton(mdl, warn, err)) {
        std::cout << "SkeletonLoaderError: " << err << std::endl;
    };
    if (!warn.empty()) {
        std::cout << "SkeletonLoaderWarning: " << warn << std::endl;
        warn.clear();
    }

    if (!ExampleExtractSkeletalAnimation(mdl, warn, err)) {
        std::cout << "AnimationLoaderError: " << err << std::endl;
    };
    if (!warn.empty()) {
        std::cout << "AnimationLoaderWarning: " << warn << std::endl;
        warn.clear();
    }

    if (!ExampleExtractSkinWeights(mdl, warn, err)) {
        std::cout << "AnimationLoaderError: " << err << std::endl;
    };
    if (!warn.empty()) {
        std::cout << "AnimationLoaderWarning: " << warn << std::endl;
        warn.clear();
    }
}