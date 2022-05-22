#include "DeferredRenderer.hpp"

#include <iostream>

#include "glad/glad.h"

namespace glrenderer
{

void DeferredRenderer::Initialize(const SendGlobalUniformCallback& SendGlobalUniformCb, uint32_t width, uint32_t height)
{
	SendGlobalUniform = SendGlobalUniformCb;
	createBuffers(width, height);
}


void DeferredRenderer::createBuffers(unsigned int width, unsigned int height)
{
	glGenFramebuffers(1, &_gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

	// - position color buffer
	glGenTextures(1, &_gPosition);
	glBindTexture(GL_TEXTURE_2D, _gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gPosition, 0);

	// - normal color buffer
	glGenTextures(1, &_gNormal);
	glBindTexture(GL_TEXTURE_2D, _gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gNormal, 0);

	// - color + specular color buffer
	glGenTextures(1, &_gColorSpec);
	glBindTexture(GL_TEXTURE_2D, _gColorSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gColorSpec, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
}

void DeferredRenderer::PreRender(const std::shared_ptr<Camera>& camera)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void DeferredRenderer::Render(entt::registry& scene, const Entity& entitySelected)
{
	geometryPass();
	//lightingPass();
}

void DeferredRenderer::PostRender()
{

}


void DeferredRenderer::geometryPass()
{


	//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	//glm::mat4 view = camera.GetViewMatrix();
	//glm::mat4 model = glm::mat4(1.0f);
	//shaderGeometryPass.use();
	//shaderGeometryPass.setMat4("projection", projection);
	//shaderGeometryPass.setMat4("view", view);
	//for (unsigned int i = 0; i < objectPositions.size(); i++)
	//{
	//	model = glm::mat4(1.0f);
	//	model = glm::translate(model, objectPositions[i]);
	//	model = glm::scale(model, glm::vec3(0.5f));
	//	shaderGeometryPass.setMat4("model", model);
	//	backpack.Draw(shaderGeometryPass);
	//}

	_geometryPassShader.Bind();



	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::lightingPass()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _gPosition);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _gNormal);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _gColorSpec);

	// also send light relevant uniforms
	//shaderLightingPass.use();
	//SendAllLightUniformsToShader(shaderLightingPass);
	//shaderLightingPass.setVec3("viewPos", camera.Position);

	renderQuad();
}

void DeferredRenderer::renderQuad()
{
	if (_quadVAO == 0)
	{
		unsigned int quadVBO = 0;
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &_quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(_quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	glBindVertexArray(_quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void DeferredRenderer::Free()
{

}

}