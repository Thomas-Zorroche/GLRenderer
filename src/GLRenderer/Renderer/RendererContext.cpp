#include "RendererContext.hpp"

#include "glad/glad.h"

#include "ForwardRenderer.hpp"
#include "DeferredRenderer.hpp"
#include "../Framebuffer.hpp"
#include "../Lighting/PointLight.hpp"


namespace glrenderer
{

RendererContext::RendererContext()
{
	SwitchRenderer(_rendererType);

	InitializeContext();
}

void RendererContext::InitializeContext()
{
	// Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Depth
	glEnable(GL_DEPTH_TEST);

	// Stencil
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	SetClearColor(glm::vec4(0.15, 0.15, 0.15, 1.0));

	flatMaterial.setShader(std::make_shared<Shader>("res/shaders/FlatColor.vert", "res/shaders/FlatColor.frag"));
	//depthMaterial.setShader(std::make_shared<Shader>("res/shaders/Depth.vert", "res/shaders/Depth.frag"));

	_renderBuffer = Framebuffer::createRenderingBuffer(64, 64); // used for main rendering (viewport)

	_shadowMap = Framebuffer::createDepthBuffer(1024, 1024);     // used for shadow mapping

	// Create Point Lights UBO
	glGenBuffers(1, &_pointLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, _pointLightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Glsl_PointLight) * _renderer->GetMaximumLightCount(), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, _pointLightsUBO);
}

void RendererContext::RenderScene(const std::shared_ptr<Camera>& camera, entt::registry& scene, const Entity& entitySelected)
{
	PreRenderContext(camera);

	_renderer->RenderScene(camera, scene, entitySelected);

	PostRenderContext();
}

void RendererContext::PreRenderContext(const std::shared_ptr<Camera>& camera)
{
	_renderBuffer->bind();
	_renderer->Clear();

	SetCamera(camera);
}

void RendererContext::PostRenderContext()
{
	_renderBuffer->unbind();
}

void RendererContext::SetCamera(const std::shared_ptr<Camera>& camera)
{
	_cameraData.viewProjectionMatrix = camera->getViewProjectionMatrix();
	_cameraData.position = camera->getPosition();
}

void RendererContext::SendGlobalUniform(const std::shared_ptr<Shader>& shader)
{
	// Camera
	shader->SetUniformMatrix4fv(GLOBAL_UNIFORM_NAME.viewProjectionMatrix, _cameraData.viewProjectionMatrix);
	shader->SetUniform3f(GLOBAL_UNIFORM_NAME.cameraPostion, _cameraData.position);

	// Shadows
	if (_shadowProperties->getComputeShadows())
	{
		shader->SetUniform1i("uSoftShadows", _shadowProperties->getSoftShadows() ? 1 : 0);

		shader->SetUniform1i("shadowMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _shadowMap->getId());

		//if (_shadowProperties->getSoftShadows())
		//{
		shader->SetUniform1i("uBlockerSearchDist", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_1D, _shadowProperties->getBlockerSearchDistribution());
		shader->SetUniform1i("uPCFFilteringDist", 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_1D, _shadowProperties->getPCFFilteringDistribution());
		shader->SetUniform1i("uBlockerSearchSamples", _shadowProperties->getBlockerSearchSamples());
		shader->SetUniform1i("uPCFFilteringSamples", _shadowProperties->getPCFSamples());
		//}
	}

	// Lighting
	shader->SetUniform1i(GLOBAL_UNIFORM_NAME.NumPointLights, _pointLightsNum);
}

void RendererContext::Free()
{
	for (auto& renderer : _rendererList)
	{
		if (renderer)
		{
			renderer->Free();
		}
	}

	_renderBuffer->free();
	_shadowMap->free();
}

void RendererContext::SwitchRenderer(ERendererType inRendererType)
{
	if (_renderer && inRendererType == _renderer->GetType())
	{
		return;
	}

	if (!_rendererList.empty())
	{
		// Search if the requested renderer has already been activated
		for (const auto& renderer : _rendererList)
		{
			if (renderer->GetType() == inRendererType)
			{
				// Stop old renderer
				if (_renderer)
					_renderer->Stop();

				// Switch renderer
				_renderer = renderer;
				// Start new renderer
				_renderer->Start();

				return;
			}
		}
	}

	// Otherwise, we need to create this renderer
	CreateRenderer(inRendererType);
	if (_renderer)
	{
		std::function<void(const std::shared_ptr<Shader>& shader)> GlobalUniformCallback = std::bind(&RendererContext::SendGlobalUniform, this, std::placeholders::_1);
		_renderer->Initialize(GlobalUniformCallback, _width, _height);
		_renderer->Start();
		_rendererList.push_back(_renderer);
	}
}

void RendererContext::CreateRenderer(ERendererType rendererType)
{
	switch (rendererType)
	{
	case glrenderer::ERendererType::FORWARD:
	{
		_renderer = std::make_shared<ForwardRenderer>(); 
		break;
	}
	case glrenderer::ERendererType::DEFERRED:
	{
		_renderer = std::make_shared<DeferredRenderer>(); 
		break;
	}
	default:
	{
		std::cout << "RendererContext::CreateRenderer error : Unknown render type" << std::endl;
		return;
	}
	}
}

void RendererContext::OnLightUpdate(const std::vector<Glsl_PointLight>& lights)
{
	if (lights.size() > _renderer->GetMaximumLightCount())
	{
		return;
	}
	
	_pointLightsNum = lights.size();
	glBindBuffer(GL_UNIFORM_BUFFER, _pointLightsUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Glsl_PointLight) * _pointLightsNum, lights.data());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RendererContext::Resize(uint32_t width, uint32_t height)
{
	_width = width;
	_height = height;
	_renderBuffer->resize(width, height);
	glViewport(0, 0, width, height);


	_renderer->Resize(width, height);
}

void RendererContext::SetClearColor(const glm::vec3& color)
{
	_clearColor = color;
	glClearColor(color.r, color.g, color.b, 1.0f);
}

unsigned int RendererContext::GetRenderBuffer() const
{
	return _renderBuffer->getTextureId();
}



}