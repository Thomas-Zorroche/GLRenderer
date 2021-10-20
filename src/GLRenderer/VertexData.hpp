#pragma once

#include <glm/glm.hpp>

namespace glrenderer {

	struct VertexData
	{
		glm::vec3 position;
		glm::vec3 normals;

		VertexData(const glm::vec3& position, const glm::vec3& normals)
			: position(position), normals(normals) {}
	};

}