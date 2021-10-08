#pragma once

#include "VertexArray.hpp"

#include <vector>
#include <memory>

namespace glrenderer {

	class Mesh
	{
	public:
		Mesh(const std::vector<float>& vertices, const std::vector<uint32_t>& indices);
		~Mesh();

		const std::shared_ptr<VertexArray>& getVertexArray() const { return _VAO; }

	private:
		void setupMesh(const std::vector<float>& vertices, const std::vector<uint32_t>& indices);

	private:
		std::shared_ptr<VertexArray> _VAO = nullptr;
	};
}