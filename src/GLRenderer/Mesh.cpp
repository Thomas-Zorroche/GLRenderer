﻿#include "Mesh.hpp"

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

#include "Shapes/ShapePlan.hpp"

namespace glrenderer {

	Mesh::Mesh(const std::vector<float>& vertices, const std::vector<uint32_t>& indices)
	{
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
			case glrenderer::MeshShape::Cube: return nullptr;
			default:				          return nullptr;
		}



	}

}