#include "Renderer.hpp"

#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace glrenderer
{

	Renderer::CameraData Renderer::_cameraData;
	Material Renderer::entitySelectedMaterial = Material(nullptr);


	void Renderer::init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		
		setClearColor(glm::vec4(0.15, 0.15, 0.15, 1.0));

		entitySelectedMaterial.setShader(std::make_shared<Shader>("res/shaders/FlatColor.vert", "res/shaders/FlatColor.frag"));
	}

	void Renderer::free()
	{

	}

	void Renderer::draw(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, 
		const glm::mat4& transform, bool selected)
	{
		if (selected)
		{
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
		}
		else
		{
			glStencilMask(0x00);
		}

		shader->Bind();
		shader->SetUniformMatrix4fv("uModelMatrix", transform);
		shader->SetUniformMatrix4fv("uProjectionMatrix", _cameraData.viewProjectionMatrix);
		shader->SetUniform3f("uCameraPos", _cameraData.position);

		vertexArray->bind();
		drawIndexed(vertexArray);

		if (selected)
		{
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			//glDisable(GL_DEPTH_TEST);

			// Draw selected entity with x1.1 scale
			auto& entitySelectedShader = entitySelectedMaterial.getShader();
			entitySelectedShader->Bind();
			entitySelectedShader->SetUniformMatrix4fv("uProjectionMatrix", _cameraData.viewProjectionMatrix);
			entitySelectedShader->SetUniformMatrix4fv("uModelMatrix", glm::scale(transform, glm::vec3(1.02f)));
			entitySelectedShader->SetUniform3f("uColor", glm::vec3(0.0f, 0.95f, 0.40f));

			drawIndexed(vertexArray);

			glStencilMask(0xFF);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			//glEnable(GL_DEPTH_TEST);
		}
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
		glStencilMask(0xFF);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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
		//glBindTexture(GL_TEXTURE_2D, 0);
	}

}