#include "Renderer.hpp"

#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "Properties/Render/ShadowsProperties.hpp"

namespace glrenderer
{

	Renderer::CameraData Renderer::_cameraData;
	Material Renderer::flatMaterial = Material(nullptr);
	Material Renderer::depthMaterial = Material(nullptr);
	std::shared_ptr<ShadowsProperties> Renderer::_shadowProperties = nullptr;

	void Renderer::init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glEnable(GL_DEPTH_TEST);

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
				
		setClearColor(glm::vec4(0.15, 0.15, 0.15, 1.0));

		flatMaterial.setShader(std::make_shared<Shader>("res/shaders/FlatColor.vert", "res/shaders/FlatColor.frag"));
		depthMaterial.setShader(std::make_shared<Shader>("res/shaders/Depth.vert", "res/shaders/Depth.frag"));
		_shadowProperties = std::make_shared<ShadowsProperties>();
	}

	void Renderer::free()
	{

	}

	void Renderer::drawDepth(const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform,
		const glm::mat4& lightMatrix)
	{ 
		auto& shader = depthMaterial.getShader();
		shader->Bind();
		shader->SetUniformMatrix4fv("uModelMatrix", transform);
		shader->SetUniformMatrix4fv("uProjectionMatrix", lightMatrix);
		glStencilMask(0x00);

		vertexArray->bind();

		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);
		drawIndexed(vertexArray);
		//glCullFace(GL_BACK); 
		//glDisable(GL_CULL_FACE);
	}

	void Renderer::draw(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, 
		const glm::mat4& transform, bool selected, unsigned int depthId)
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

		// RENDER UNIFORMS
		shader->Bind();
		shader->SetUniformMatrix4fv("uModelMatrix", transform);
		shader->SetUniformMatrix4fv("uProjectionMatrix", _cameraData.viewProjectionMatrix);
		shader->SetUniform3f("uCameraPos", _cameraData.position);
		shader->SetUniform1i("uSoftShadows", _shadowProperties->getSoftShadows());
		
		shader->SetUniform1i("shadowMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthId);

		if (_shadowProperties->getSoftShadows())
		{
			shader->SetUniform1i("uBlockerSearchDist", 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_1D, _shadowProperties->getBlockerSearchDistribution());
			shader->SetUniform1i("uPCFFilteringDist", 2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_1D, _shadowProperties->getPCFFilteringDistribution());
			shader->SetUniform1i("uBlockerSearchSamples", _shadowProperties->getBlockerSearchSamples());
			shader->SetUniform1i("uPCFFilteringSamples", _shadowProperties->getPCFSamples());
		}

		vertexArray->bind();
		drawIndexed(vertexArray);

		if (selected)
		{
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			//glDisable(GL_DEPTH_TEST);

			// Draw selected entity with x1.1 scale
			auto& entitySelectedShader = flatMaterial.getShader();
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

	void Renderer::drawLine(const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform, bool selected)
	{
		// TODO move material and color constant to SCENE
		// renderer should only receive VAO, Transform and shader
		auto& shader = flatMaterial.getShader();
		shader->Bind();
		shader->SetUniformMatrix4fv("uProjectionMatrix", _cameraData.viewProjectionMatrix);
		shader->SetUniformMatrix4fv("uModelMatrix", transform);
		shader->SetUniform3f("uColor", selected ? glm::vec3(0.0f, 0.95f, 0.40f) : glm::vec3(0.0f, 0.0f, 0.0f));

		glStencilMask(0x00);
		vertexArray->bind();
		glDrawArrays(GL_LINES, 0, 2);
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

	void Renderer::setCamera(const glm::mat4& view, const glm::vec3& cameraPosition)
	{
		_cameraData.viewProjectionMatrix = view;
		_cameraData.position = cameraPosition;
	}


	// Private Function

	void Renderer::drawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
		uint32_t count = vertexArray->getIndexBuffer() ? vertexArray->getIndexBuffer()->getCount() : 0;

		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
		//glBindTexture(GL_TEXTURE_2D, 0);
	}

}