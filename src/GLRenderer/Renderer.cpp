#include "Renderer.hpp"

#include <glad/glad.h>
#include <iostream>

namespace glrenderer
{

	glm::mat4 Renderer::_viewProjectionMatrix = glm::mat4(1.0f);

	void Renderer::init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		
		setClearColor(glm::vec4(1, 0.737, 0.019, 1.0));
	}

	void Renderer::free()
	{

	}

	void Renderer::updateCamera()
	{
		_viewProjectionMatrix = glm::mat4(1.0f);
	}

	void Renderer::draw(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, 
		const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetUniformMatrix4fv("uModelMatrix", transform);
		shader->SetUniformMatrix4fv("uProjectionMatrix", _viewProjectionMatrix);

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

	void Renderer::setCamera(const glm::mat4& viewProjectionMatrix)
	{
		_viewProjectionMatrix = viewProjectionMatrix;
	}

	// Private Function

	void Renderer::drawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
		uint32_t count = vertexArray->getIndexBuffer() ? vertexArray->getIndexBuffer()->getCount() : 0;

		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}