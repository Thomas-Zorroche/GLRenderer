#pragma once

#include "glm/glm.hpp"
#include <memory>

#include "Shader.h"
#include "VertexArray.hpp"
#include "Camera.hpp"


namespace glrenderer {

	class Renderer
	{
	public:
		static void init();

		static void free();

		static void draw(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader,
			const glm::mat4& transform);
		
		static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		static void setClearColor(const glm::vec4& color);

		static void setCamera(const std::shared_ptr<Camera>& camera);

		static void clear();

	private:
		static void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray);

	private:
		struct CameraData
		{
			glm::vec3 position;
			glm::mat4 viewProjectionMatrix;
		};
		static CameraData _cameraData;
	};
}