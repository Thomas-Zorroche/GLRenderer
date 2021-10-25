#pragma once

#include "glm/glm.hpp"
#include <memory>

#include "Shader.h"
#include "Material.hpp"
#include "VertexArray.hpp"
#include "Camera.hpp"


namespace glrenderer {

	class Renderer
	{
	public:
		static void init();

		static void free();

		static void draw(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader,
			const glm::mat4& transform, bool selected = false, unsigned int depthId = 0);

		static void drawDepth(const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform,
			const glm::mat4& lightMatrix);

		static void drawLine(const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform, bool selected = false);
		
		static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		static void setClearColor(const glm::vec4& color);

		static void clear();
		
		static void setCamera(const std::shared_ptr<Camera>& camera);
		static void setCamera(const glm::mat4& view, const glm::vec3& cameraPosition);


	private:
		static void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray);

	private:
		struct CameraData
		{
			glm::vec3 position;
			glm::mat4 viewProjectionMatrix;
		};
		static CameraData _cameraData;

		// TEMP
		static Material flatMaterial;
		static Material depthMaterial;
	};
}