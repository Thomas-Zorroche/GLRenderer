#include "Renderer.hpp"

#include <glad/glad.h>
#include <iostream>

namespace glrenderer
{

	Renderer::CameraData Renderer::_cameraData;

	void Renderer::init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		
		setClearColor(glm::vec4(0.15, 0.15, 0.15, 1.0));
	}

	void Renderer::free()
	{

	}

	void Renderer::draw(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, 
		const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetUniformMatrix4fv("uModelMatrix", transform);
		shader->SetUniformMatrix4fv("uProjectionMatrix", _cameraData.viewProjectionMatrix);
		shader->SetUniform3f("uCameraPos", _cameraData.position);

		vertexArray->bind();
		drawIndexed(vertexArray);
	}

	void Renderer::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void Renderer::setClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void Renderer::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::setCamera(const std::shared_ptr<Camera>& camera)
	{
		_cameraData.viewProjectionMatrix = camera->getViewProjectionMatrix();
		_cameraData.position = camera->getPosition();
	}

	// Private Function

	void Renderer::drawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
		uint32_t count = vertexArray->getIndexBuffer() ? vertexArray->getIndexBuffer()->getCount() : 0;

		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}