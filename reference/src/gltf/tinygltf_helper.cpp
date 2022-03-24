#define TINYGLTF_IMPLEMENTATION

#include "gltf/tinygltf_helper.h"

tinygltf_DataGetter tinygltf_buildDataGetter(const tinygltf::Model& gltf, int accIndex)
{
    const tinygltf::Accessor& acc = gltf.accessors[accIndex];
    const tinygltf::BufferView& bv = gltf.bufferViews[acc.bufferView];
    const tinygltf::Buffer& buf = gltf.buffers[bv.buffer];

    const unsigned char* data = &(buf.data[bv.byteOffset + acc.byteOffset]);
    size_t len = acc.count;
    int stride = acc.ByteStride(bv);
    int size = tinygltf::GetComponentSizeInBytes(acc.componentType) *
               tinygltf::GetNumComponentsInType(acc.type);

    // std::cout << len << " " << stride << " " << size << std::endl;
    
    return { data, len, stride, size };
}

int tinygltf_findAccessor(
    const tinygltf::Primitive& geom, 
    const std::string& name
) {
    auto it = geom.attributes.find(name);
    if(it == geom.attributes.end())
        return -1;
    return it->second;
}

bool tinygltf_parsefile(const std::string& filename, tinygltf::Model& gltf) {
    auto loader = std::make_unique<tinygltf::TinyGLTF>();

    std::string err, warn;

    size_t extIndex = filename.find('.');
    if (extIndex == std::string::npos || extIndex >= filename.length() - 1) {
        err = "Filename specified incorrectly - no extension!";
        return false;
    }

    std::string ext = filename.substr(extIndex + 1, std::string::npos);
    if (ext != "gltf" && ext != "glb") {
        err = "Filename specified incorrectly - not a GLTF or GLB!";
        return false;
    }

    bool result = (ext == "glb") ? 
          loader->LoadBinaryFromFile(&gltf, &err, &warn, filename.c_str())
        : loader->LoadASCIIFromFile(&gltf, &err, &warn, filename.c_str());

    if (!result) {
        std::cout << "Failed to load gltf file: " << filename.c_str();
    }

    if (!warn.empty()) {
        std::cout << "GLTFLoaderWarning: " << warn << std::endl;
    }
    if (!err.empty()) {
        std::cout << "GLTFLoaderError: " << err << std::endl;
    }
    std::cout<<"Load model successful."<<std::endl;
    return result;
}
