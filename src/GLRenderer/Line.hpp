#pragma once

#include <glm/glm.hpp>

#include "VertexArray.hpp"


namespace glrenderer {

	class Line
	{
	public:
		Line(const glm::vec3& start = { 0.0, 0.0, 0.0 }, const glm::vec3& end = { 0.0, -10.0, 0.0 });

		const std::shared_ptr<VertexArray>& getVertexArray() const { return _VAO; }

		glm::vec3 getDirection() const;

	private:
		glm::vec3 _start;
		glm::vec3 _end;

		std::shared_ptr<VertexArray> _VAO = nullptr;

		glm::vec3 _direction;

	};

}