#pragma once

#include "glm/glm.hpp"
#include <memory>

#include "Shader.h"

namespace glrenderer {

	class Renderer
	{
	public:
		static void init();

		static void free();

		static void updateCamera();

		static void submit(const std::shared_ptr<Shader>& shader, const glm::mat4& transform = glm::mat4(1.0f));
		
		static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		static void setClearColor(const glm::vec4& color);

		static void clear();

	private:
		static void drawIndexed(uint32_t indexCount);

	private:
		static glm::mat4 _viewProjectionMatrix;
	};
}