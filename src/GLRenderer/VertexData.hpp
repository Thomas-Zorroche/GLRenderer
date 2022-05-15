#pragma once

#include <glm/glm.hpp>

namespace glrenderer {

	struct VertexData
	{
		glm::vec3 position;
		glm::vec3 normals;
		glm::vec2 texcoords;

		VertexData(const glm::vec3& position, const glm::vec3& normals, const glm::vec2& texcoords)
			: position(position), normals(normals), texcoords(texcoords) {}

		VertexData(const glm::vec3& position, const glm::vec3& normals)
			: position(position), normals(normals), texcoords(glm::vec2()) {}
		
		VertexData(const glm::vec3& position)
			: position(position), normals(glm::vec3()), texcoords(glm::vec2()) {}

		VertexData()
			: position(glm::vec3()), normals(glm::vec3()), texcoords(glm::vec2()) {}
	};

}