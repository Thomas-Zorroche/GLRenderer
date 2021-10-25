#include "Line.hpp"
#include <glad/glad.h>

#include <vector>

#include "VertexBuffer.hpp"
#include "VertexData.hpp"


namespace glrenderer {

	Line::Line(const glm::vec3& start, const glm::vec3& end)
		: _start(start), _end(end), _direction(end - start)
	{
		std::vector<VertexData> vertices = {
			{ {start.x, start.y, start.z} },
			{ {end.x, end.y, end.z} }
		};

		_VAO = std::make_shared<glrenderer::VertexArray>();

		const auto& VBO = std::make_shared<VertexBuffer>(vertices);
		glrenderer::BufferLayout layout = {
			{
				glrenderer::BufferAttribute(), // position
				glrenderer::BufferAttribute()
			}
		};
		VBO->setLayout(layout);
		_VAO->setVertexBuffer(VBO);
	}

	glm::vec3 Line::getDirection() const
	{
		return _direction;
	}


}