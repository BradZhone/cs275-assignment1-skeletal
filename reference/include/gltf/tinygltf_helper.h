/**
 * Some helper functions to load data from *.gltf/*.glb files
 * with tinygltf library. You can modify whatever you want
 * 
 * You can also use other file formats or libraries to load your 
 * favorite mesh, skeleton, skinning weights and animations
 * 
 * Check the gltf 2.0 specification if you feel confused
 * https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html
 * */
#pragma once

#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE 
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE 
#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_EXTERNAL_IMAGE

#include <nlohmann/json.hpp>
#include <tiny_gltf.h>
#include <iostream>

struct tinygltf_DataGetter
{
    const unsigned char* data;
    size_t len;
    int stride;
    int elementSize;
};

tinygltf_DataGetter tinygltf_buildDataGetter(const tinygltf::Model& gltf, int accIndex);
int tinygltf_findAccessor(const tinygltf::Primitive& geom, const std::string& name);
bool tinygltf_parsefile(const std::string& filename, tinygltf::Model& gltf);