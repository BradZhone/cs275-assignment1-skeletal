#include "skeletal/mesh.hpp"

#include <glad/glad.h>

#include "gltf/tinygltf_helper.h"

bool BoneWeightedMesh::
loadFromTinyGLTF(
    const tinygltf::Model& mdl, 
    std::string& warn, 
    std::string& err
) { 
    if (mdl.meshes.size() == 0) {
        err = "No meshes in file.";
        return false;
    }
    
    const tinygltf::Mesh& meshData = mdl.meshes[0];
    
    if (meshData.primitives.size() == 0) {
        err = "No geometry data associated with mesh.";
        return false;
    }

    bool flipUVY = false;
    for (size_t geomIndex = 0; geomIndex < meshData.primitives.size(); ++geomIndex) {
        const tinygltf::Primitive geom = mdl.meshes[0].primitives[geomIndex];

        if (geom.indices == -1) {
            err = "File is missing primitive indices. " \
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
        auto faceIndexer = tinygltf_buildDataGetter(mdl, geom.indices);
        if (faceIndexer.elementSize != sizeof(GLshort)) {
            err = "Primitive indices are in a currently unsupported format. " \
                  "Consider changing your GLTF export settings, or else this loader " \
                  "must be augmented to support the provided format.";
            return false;
        }
        
        int influenceID = tinygltf_findAccessor(geom, "JOINTS_0");
        if (influenceID == -1) {
            err = "No joint influence data present in file.";
            return false;
        }
        int weightID = tinygltf_findAccessor(geom, "WEIGHTS_0");
        if (weightID == -1) {
            err = "No skin weight data present in file.";
            return false;
        }
        int vID = tinygltf_findAccessor(geom, "POSITION");
        if (vID == -1) {
            err = "No vertex positions found in mesh primitive " + std::to_string(geomIndex);
            return false;
        }
        int nID = tinygltf_findAccessor(geom, "NORMAL");
        this->hasNormals = nID != -1;
        if (!this->hasNormals) {
            warn += "\nNo normals found in mesh primitive " + std::to_string(geomIndex);
        }
        int uvID = tinygltf_findAccessor(geom, "TEXCOORD_0");
        this->hasUVs = uvID != -1;
        if (!this->hasUVs) {
            warn += "\nNo UVs found in mesh primitive " + std::to_string(geomIndex);
        }

        tinygltf_DataGetter infGetter, wtGetter, vGetter, nGetter, uvGetter;
        
        infGetter = tinygltf_buildDataGetter(mdl, influenceID);
            if (infGetter.elementSize != 4 * sizeof(GLubyte)) {
            err = "Joint influences are in a currently unsupported format." \
                  "Consider changing your GLTF export settings, or else check for " \
                  "and support this format in your GLTF loader implementation.";

            return false;
        }
        wtGetter = tinygltf_buildDataGetter(mdl, weightID);
        if (wtGetter.elementSize != 4 * sizeof(GLfloat)) {
            err = "Skin weights are in a currently unsupported format." \
                  "Consider changing your GLTF export settings, or else check for " \
                  "and support this format in your GLTF loader implementation.";

            return false;
        }
        vGetter = tinygltf_buildDataGetter(mdl, vID);
        if (vGetter.elementSize != sizeof(glm::vec3)) {
            err = "Vertex position data is in a currently unsupported format. " \
                  "Consider changing your GLTF export settings, or else this loader " \
                  "must be augmented to support the provided format.";
            return false;
        }
        if (this->hasNormals) {
            nGetter = tinygltf_buildDataGetter(mdl, nID);
            if (nGetter.elementSize != sizeof(glm::vec3)) {
                this->hasNormals = false;
                warn += "\nNormal data is in a currently unsupported format. " \
                        "Consider changing your GLTF export settings, or else this loader " \
                        "must be augmented to support the provided format.";
            }
        }
        if (this->hasUVs) {
            uvGetter = tinygltf_buildDataGetter(mdl, uvID);
            if (uvGetter.elementSize != sizeof(glm::vec2)) {
                this->hasUVs = false;
                warn += "\nUV data is in a currently unsupported format. " \
                        "Consider changing your GLTF export settings, or else this loader " \
                        "must be augmented to support the provided format.";
            }
        }

        size_t indiceOffset = this->indices.size();
        size_t startIndex = this->positions.size();

        this->indices.resize(indiceOffset + faceIndexer.len);
        this->influences.resize(startIndex + infGetter.len);
        this->weights.resize(startIndex + wtGetter.len);
        this->positions.resize(startIndex + vGetter.len);
        if (this->hasNormals) {
            this->normals.resize(startIndex + nGetter.len);
        }
        if (this->hasUVs) {
            this->uvs.resize(startIndex + uvGetter.len);
        }

        // This is the bit where we actually get to extracting our data.
        for (size_t i = 0; i < faceIndexer.len; ++i)
        {
            // What vertex do we need to look at?
            GLshort vertIndex;
            memcpy(&vertIndex, &faceIndexer.data[i * faceIndexer.stride], sizeof(GLshort));
            this->indices[indiceOffset + i] = startIndex + vertIndex;
        }

        // Note: glTF appears to "switch" between how joints are indexed
        // when it comes to skin weights - the joint IDs will match
        // the ORDER of nodes in the skeleton definition, rather than the
        // IDENTIFIERS of those nodes.
        const tinygltf::Skin& skin = mdl.skins[0];
        for (size_t i = 0; i < infGetter.len; ++i) {
            unsigned char j0, j1, j2, j3; // these are index of `skin.joints`
            memcpy(&j0, &infGetter.data[i * infGetter.stride + 0 * sizeof(GLubyte)], sizeof(GLubyte));
            memcpy(&j1, &infGetter.data[i * infGetter.stride + 1 * sizeof(GLubyte)], sizeof(GLubyte));
            memcpy(&j2, &infGetter.data[i * infGetter.stride + 2 * sizeof(GLubyte)], sizeof(GLubyte));
            memcpy(&j3, &infGetter.data[i * infGetter.stride + 3 * sizeof(GLubyte)], sizeof(GLubyte));
            
            this->influences[startIndex + i] = glm::uvec4(
                skin.joints[j0], 
                skin.joints[j1], 
                skin.joints[j2], 
                skin.joints[j3]
            ); // these are index from `mdl.nodes`
        }

        for (size_t i = 0; i < wtGetter.len; ++i) {
            memcpy(&this->weights[startIndex + i], &wtGetter.data[i * wtGetter.stride], sizeof(glm::vec4));
        }

        for (size_t i = 0; i < vGetter.len; ++i) {
            memcpy(&this->positions[startIndex + i], &vGetter.data[i * vGetter.stride], sizeof(glm::vec3)); // grab vertex position.
        }
        if (this->hasNormals) {
            for (size_t i = 0; i < nGetter.len; ++i) {
                memcpy(&this->normals[startIndex + i], &nGetter.data[i * nGetter.stride], sizeof(glm::vec3)); // grab vertex normal.
            }
        }    
        if (this->hasUVs) {
            for (size_t i = 0; i < uvGetter.len; ++i) {
                memcpy(&this->uvs[startIndex + i], &uvGetter.data[i * uvGetter.stride], sizeof(glm::vec2)); //grab texture coordinates.
                // We may need to flip our vertical UV-coordinate.
                // You will probably need to do this, depending on your export settings/texture.
                if (flipUVY) {
                    this->uvs[startIndex + i].y = 1.0f - this->uvs[startIndex + i].y;
                }
            }
        }
    }

    return true;
}