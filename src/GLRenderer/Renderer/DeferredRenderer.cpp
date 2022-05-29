#include "DeferredRenderer.hpp"

#include <iostream>

#include "glad/glad.h"

#include "../Scene/Component.hpp"

namespace glrenderer
{

DeferredRenderer::DeferredRenderer(RendererData rendererData, RendererContext* renderContext)
	: ARendererBase(rendererData, renderContext)
{

}


void DeferredRenderer::Initialize()
{
	_geometryPassShader = std::make_shared<Shader>("res/shaders/DeferredRendering/GeometryPass.vert", "res/shaders/DeferredRendering/GeometryPass.frag");
	_lightingPassShader = std::make_shared<Shader>("res/shaders/DeferredRendering/LightingPass.vert", "res/shaders/DeferredRendering/LightingPass.frag");

	_geometryPassShader->Bind();
	{
		_geometryPassShader->SetUniform1i("uBaseColorTexture", 0);

		unsigned int uniformBlockIndex = glGetUniformBlockIndex(_geometryPassShader->getID(), "Lights");
		glUniformBlockBinding(_geometryPassShader->getID(), uniformBlockIndex, 0);
	}
	_geometryPassShader->Unbind();

	_lightingPassShader->Bind();
	{
		_lightingPassShader->SetUniform1i("gPosition", 0);
		_lightingPassShader->SetUniform1i("gNormal", 1);
		_lightingPassShader->SetUniform1i("gAlbedoSpec", 2);
	}
	_lightingPassShader->Unbind();

	createBuffers(_rendererData._renderBufferWidth, _rendererData._renderBufferHeight);
}


void DeferredRenderer::createBuffers(unsigned int width, unsigned int height)
{
	glGenFramebuffers(1, &_gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
	
	// - position color buffer
	glGenTextures(1, &_gPosition);
	glBindTexture(GL_TEXTURE_2D, _gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _rendererData._renderBufferWidth, _rendererData._renderBufferHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gPosition, 0);
	
	// - normal color buffer
	glGenTextures(1, &_gNormal);
	glBindTexture(GL_TEXTURE_2D, _gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _rendererData._renderBufferWidth, _rendererData._renderBufferHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gNormal, 0);
	
	// - color + specular color buffer
	glGenTextures(1, &_gColorSpec);
	glBindTexture(GL_TEXTURE_2D, _gColorSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _rendererData._renderBufferWidth, _rendererData._renderBufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gColorSpec, 0);
	
	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	
	// create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &_rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, _rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _rendererData._renderBufferWidth, _rendererData._renderBufferHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete : " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::PreRender(const std::shared_ptr<Camera>& camera)
{

}

void DeferredRenderer::Render(entt::registry& scene, const Entity& entitySelected)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		geometryPass(scene);
	}

	// Default Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, _rendererData.RENDER_BUFFER);
	{
		glClearColor(0.141f, 0.207f, 0.211f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightingPass();
	}
}

void DeferredRenderer::PostRender()
{

}


void DeferredRenderer::geometryPass(entt::registry& scene)
{
	_geometryPassShader->Bind();

	// Render Meshes
	auto group = scene.group<TransformComponent>(entt::get<MeshComponent>);
	for (const auto entity : group)
	{
		auto [transform, meshComp] = group.get<TransformComponent, MeshComponent>(entity);

		const std::shared_ptr<Mesh>& mesh = meshComp.mesh;
		if (!mesh->IsVisible())
		{
			continue;
		}

		const std::shared_ptr<Material>& material = mesh->getMaterial();
		// TODO : forward pass for wireframe objects
		if (material->IsWireframe())
		{
			continue;
		}

		material->bindColorTexture();

		SendCameraUniforms(_geometryPassShader);
		SendModelUniform(_geometryPassShader, transform.getModelMatrix());

		DrawVAO(mesh->getVertexArray());
	}
}

void DeferredRenderer::lightingPass()
{
	/*
	* use different shader for different viewport display type
	*/
	_lightingPassShader->Bind();

	SendLightingUniforms(_lightingPassShader);
	SendCameraUniforms(_lightingPassShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _gPosition);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _gNormal);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _gColorSpec);

	// Rendering Quad
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void DeferredRenderer::Resize(uint32_t width, uint32_t height)
{
	glBindTexture(GL_TEXTURE_2D, _gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _rendererData._renderBufferWidth, _rendererData._renderBufferHeight, 0, GL_RGBA, GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_2D, _gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _rendererData._renderBufferWidth, _rendererData._renderBufferHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, _gColorSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _rendererData._renderBufferWidth, _rendererData._renderBufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindRenderbuffer(GL_RENDERBUFFER, _rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _rendererData._renderBufferWidth, _rendererData._renderBufferHeight);
}

void DeferredRenderer::SwitchViewportBuffer(int bufferID)
{
	//EViewportBuffer bufferViewerType = static_cast<EViewportBuffer>(bufferID);

	//switch (bufferViewerType)
	//{
	//case glrenderer::EViewportBuffer::Lighting:
	//	break;
	//case glrenderer::EViewportBuffer::Color:
	//	break;
	//case glrenderer::EViewportBuffer::Spec:
	//	break;
	//case glrenderer::EViewportBuffer::Position:
	//	break;
	//case glrenderer::EViewportBuffer::Normal:
	//	break;
	//default:
	//	break;
	//}

}

unsigned int DeferredRenderer::GetBufferTextureId(EViewportBufferState Buffer)
{
	switch (Buffer)	
	{
	case glrenderer::EViewportBufferState::Color:		return _gColorSpec;
	case glrenderer::EViewportBufferState::Spec:		return _gColorSpec;
	case glrenderer::EViewportBufferState::Position:	return _gPosition;
	case glrenderer::EViewportBufferState::Normal:		return _gNormal;
	default:											return _gColorSpec;
	}
}

void DeferredRenderer::AddViewportBufferInList(std::vector<std::string>& buffers)
{
	buffers.push_back("-");
	buffers.push_back("Color");
	buffers.push_back("Spec");
	buffers.push_back("Position");
	buffers.push_back("Normal");
}

void DeferredRenderer::Free()
{
	glDeleteBuffers(1, &_gBuffer);
	glDeleteBuffers(1, &_quadVAO);
	glDeleteBuffers(1, &_quadVBO);

	glDeleteTextures(1, &_gNormal);
	glDeleteTextures(1, &_gPosition);
	glDeleteTextures(1, &_gColorSpec);
}

}