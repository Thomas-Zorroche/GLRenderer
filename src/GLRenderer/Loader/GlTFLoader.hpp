#pragma once

#include "tinygltf/tiny_gltf.h"
#include "../Mesh.hpp"

#include <string>
#include <vector>

namespace glTFLoader
{
    template<typename T>
    void getTinyGlTFBuffer(tinygltf::Model& model, size_t index_mesh, size_t index_primitive, const std::string& bufferName, std::vector<T>& buffer);
    
    void buildVertexDataFromTinyGlTFBuffer(const std::vector<float>& position, const std::vector<float>& normals,
        const std::vector<float>& texcoords, std::vector<glrenderer::VertexData>& vertices);
    
    void buildIndicesFromTinyGlTFBuffer(const std::vector<unsigned short>& indicesBuffer, std::vector<uint32_t>& indices);

	int loadFile(const std::string& filePath, tinygltf::Model& model);

	void createMeshes(tinygltf::Model& model, std::vector<std::shared_ptr<glrenderer::Mesh>>& meshes);


}