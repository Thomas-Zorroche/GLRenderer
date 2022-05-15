#include "GlTFLoader.hpp"

#include "tinygltf/json.hpp"
#include "tinygltf/stb_image.h"
#include "tinygltf/stb_image_write.h"

#include <glad/glad.h>

#include <memory>
#include <iostream>

#include <cstring>
#include <stdexcept>
#include <type_traits>

namespace glTFLoader
{
    int loadFile(const std::string& filePath, tinygltf::Model& model)
	{
        tinygltf::TinyGLTF loader;
        std::string error;
        std::string warning;

        bool ret = loader.LoadASCIIFromFile(&model, &error, &warning, filePath);

        if (!warning.empty())
            std::cout << "glTFLoader::loadFile warning: " << warning << std::endl;

        if (!error.empty())
            std::cout << "glTFLoader::loadFile error : " << error << std::endl;

        if (!ret) 
        {
            std::cout << "glTFLoader::loadFile Failed to parse glTF" << std::endl;
            return false;
        }

        return true;
	}

    void createMeshes(tinygltf::Model& model, std::vector<std::shared_ptr<glrenderer::Mesh>>& meshes)
    {
        const GLuint VERTEX_ATTRIB_POSITION_IDX = 0;
        const GLuint VERTEX_ATTRIB_NORMAL_IDX = 1;
        const GLuint VERTEX_ATTRIB_TEXCOORD0_IDX = 2;
        const GLuint VERTEX_ATTRIB_TANGENT_IDX = 3;

        // For now, one mesh for each primitive, because a mesh can only possess one unique VAO

        // TinyGlTF mesh data
        std::vector<float> positionBuffer;
        std::vector<float> normalBuffer;
        std::vector<float> texcoordsBuffer;
        std::vector<unsigned short> indicesBuffer;

        // GLRenderer mesh data
        std::vector<glrenderer::VertexData> vertices;
        std::vector<uint32_t> indices;
        std::shared_ptr<glrenderer::Material> material = nullptr;

        size_t meshesCount = model.meshes.size();
        for (int index_mesh = 0; index_mesh < meshesCount; index_mesh++)
        {
            size_t primitivesCount = model.meshes[index_mesh].primitives.size();
            for (int index_primitive = 0; index_primitive < primitivesCount; index_primitive++)
            {
                // Reset mesh data
                vertices.clear();
                indices.clear();
                material = nullptr;

                positionBuffer.clear();
                normalBuffer.clear();
                texcoordsBuffer.clear();
                indicesBuffer.clear();

                // Retrieve TinyGlTF mesh data
                getTinyGlTFBuffer(model, index_mesh, index_primitive, "POSITION", positionBuffer);
                getTinyGlTFBuffer(model, index_mesh, index_primitive, "NORMAL", normalBuffer);
                getTinyGlTFBuffer(model, index_mesh, index_primitive, "TEXCOORD_0", texcoordsBuffer);

                // Retrieve indices
                if (model.meshes[index_mesh].primitives[index_primitive].indices >= 0)
                {
                    tinygltf::Accessor indicesAccessor = model.accessors[model.meshes[index_mesh].primitives[index_primitive].indices];
                    int indicesBufferID = model.bufferViews[indicesAccessor.bufferView].buffer;
                    getTinyGlTFBuffer(model, index_mesh, index_primitive, "INDICES", indicesBuffer);
                }

                buildVertexDataFromTinyGlTFBuffer(positionBuffer, normalBuffer, texcoordsBuffer, vertices);
                buildIndicesFromTinyGlTFBuffer(indicesBuffer, indices);

                // TODO create material

                auto mesh = std::make_shared<glrenderer::Mesh>(vertices, indices, material);
                meshes.push_back(mesh);
            }
        }
    }

    // https://stackoverflow.com/questions/52410302/convert-n-first-bytes-of-unsigned-char-pointer-to-float-and-double-c
    struct ByteStreamReader {
        unsigned char const* begin;
        unsigned char const* const end;

        template<class T>
        operator T() {
            static_assert(std::is_trivially_copyable<T>::value,
                "The type you are using cannot be safely copied from bytes.");
            if (end - begin < static_cast<decltype(end - begin)>(sizeof(T)))
                throw std::runtime_error("ByteStreamReader");
            T t;
            std::memcpy(&t, begin, sizeof t);
            begin += sizeof t;
            return t;
        }
    };

    static inline size_t GetComponentSizeInBytes(uint32_t componentType) {
        if (componentType == TINYGLTF_COMPONENT_TYPE_BYTE) {
            return 1;
        }
        else if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
            return 1;
        }
        else if (componentType == TINYGLTF_COMPONENT_TYPE_SHORT) {
            return 2;
        }
        else if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
            return 2;
        }
        else if (componentType == TINYGLTF_COMPONENT_TYPE_INT) {
            return 4;
        }
        else if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
            return 4;
        }
        else if (componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
            return 4;
        }
        else if (componentType == TINYGLTF_COMPONENT_TYPE_DOUBLE) {
            return 8;
        }
        else {
            // Unknown componenty type
            return -1;
        }
    }

    template<typename T>
    void getTinyGlTFBuffer(tinygltf::Model& model, size_t index_mesh, size_t index_primitive, const std::string& bufferName, std::vector<T>& out_buffer)
    {
        if (bufferName != "INDICES")
        {
            const auto iteratorPos = model.meshes[index_mesh].primitives[index_primitive].attributes.find(bufferName);
            if (iteratorPos == end(model.meshes[index_mesh].primitives[index_primitive].attributes))
            {
                // TODO : some model do not have tex coords (white texture)
                std::cerr << "getTinyGlTFBuffer error: " << bufferName << " not found." << std::endl;
                return;
            }
        }

        const tinygltf::Primitive& primitive = model.meshes[index_mesh].primitives[index_primitive];
        const tinygltf::Accessor accessor = (bufferName != "INDICES") ? model.accessors[primitive.attributes.at(bufferName)] : model.accessors[primitive.indices];
        const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

        if (bufferName != "INDICES")
        {
            out_buffer.resize(accessor.count * accessor.type);

            const float* positions = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
            uint32_t vertex_count = static_cast<uint32_t>(accessor.count);
            for (size_t vindex = 0; vindex < vertex_count; ++vindex)
            {
                for (size_t j = 0; j < accessor.type; j++)
                    out_buffer[vindex * accessor.type + j] = positions[vindex * accessor.type + j];
            }
        }
        else
        {
            out_buffer.resize(accessor.count);

            const unsigned short* indices = reinterpret_cast<const unsigned short*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
            for (size_t i = 0; i < accessor.count; ++i) 
                out_buffer[i] = indices[i];
        }

        return;

        // TODO replace assert by LOG_GLRENDERER_ERROR
        //assert(sizeof T == GetComponentSizeInBytes(accessor.componentType)
        //    && printf("[GlTFLoader] error : GlTF file ComponentTypeSize %zu bytes != %zu bytes \n", GetComponentSizeInBytes(accessor.componentType), sizeof T));
    }

    void buildVertexDataFromTinyGlTFBuffer(const std::vector<float>& position, const std::vector<float>& normals, 
        const std::vector<float>& texcoords, std::vector<glrenderer::VertexData>& vertices)
    {
        uint32_t vertexCount = 0;
        uint32_t texcoordsIndex = 0; // TEMP
        vertices.resize(position.size() / 3);
        for (size_t i = 0; i + 2 < position.size(); i+=3)
        {
            vertices[vertexCount] = {
                glm::vec3(position[i], position[i + 1], position[i + 2]),
                glm::vec3(normals[i], normals[i + 1], normals[i + 2]),
                //glm::vec2(texcoords[texcoordsIndex++], texcoords[texcoordsIndex++]),
                glm::vec2(0, 0)
            };
            vertexCount++;
        }
    }

    void buildIndicesFromTinyGlTFBuffer(const std::vector<unsigned short>& indicesBuffer, std::vector<uint32_t>& indices)
    {
        indices.resize(indicesBuffer.size());
        for (size_t i = 0; i < indicesBuffer.size(); i++)
        {
            indices[i] = (uint32_t)indicesBuffer[i];
        }
    }
}