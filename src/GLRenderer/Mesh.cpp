#include "Mesh.hpp"

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

#include "Shapes.hpp"

namespace glrenderer {

	Mesh::Mesh(const std::vector<float>& vertices, const std::vector<uint32_t>& indices, const std::shared_ptr<Material>& material)
		: _material(material)
	{
		if (material == nullptr)
		{
			_material = std::make_shared<Material>(std::make_shared<Shader>("res/shaders/FlatColor.vert", "res/shaders/FlatColor.frag"));
		}
		setupMesh(vertices, indices);
	}

	Mesh::~Mesh()
	{
	
	}

	void Mesh::setupMesh(const std::vector<float>& vertices, const std::vector<uint32_t>& indices)
	{
		_VAO = std::make_shared<glrenderer::VertexArray>();

		// Vertex Buffer
		const auto& VBO = std::make_shared<VertexBuffer>(vertices);
		glrenderer::BufferLayout layout = {
			{glrenderer::BufferAttribute()} // position
		};
		VBO->setLayout(layout);
		_VAO->setVertexBuffer(VBO);

		// Index Buffer
		auto IBO = std::make_shared<glrenderer::IndexBuffer>(indices, indices.size());
		_VAO->setIndexBuffer(IBO);
	}

	std::shared_ptr<Mesh> Mesh::createMesh(MeshShape mesh)
	{
		switch (mesh)
		{
			case glrenderer::MeshShape::Plan:
			{
				ShapePlan plan;
				return std::make_shared<Mesh>(plan.vertices, plan.indices);
			}
			case glrenderer::MeshShape::Cube:
			{
				ShapeCube cube;
				return std::make_shared<Mesh>(cube.vertices, cube.indices);
			}
			default: return nullptr;
		}



	}

}