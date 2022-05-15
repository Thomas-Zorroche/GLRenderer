#pragma once

#include "VertexArray.hpp"
#include "Material.hpp"
#include "VertexData.hpp"

#include <vector>
#include <memory>

namespace glrenderer {

	enum class MeshShape
	{
		Plan = 0,
		Cube
	};

	class Mesh
	{
	public:
		Mesh(const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices, const std::shared_ptr<Material>& material = nullptr);

		~Mesh();

		const std::shared_ptr<VertexArray>& getVertexArray() const { return _VAO; }
		
		std::shared_ptr<Material>& getMaterial() { return _material; }

		static std::shared_ptr<Mesh> createMesh(MeshShape mesh);

	private:
		void setupMesh(const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices);

	private:
		std::shared_ptr<VertexArray> _VAO = nullptr;
		std::shared_ptr<Material> _material;
	};
}