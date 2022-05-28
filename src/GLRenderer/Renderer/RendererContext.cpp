#include "RendererContext.hpp"

#include "glad/glad.h"

#include "ForwardRenderer.hpp"
#include "DeferredRenderer.hpp"
#include "../Framebuffer.hpp"
#include "../Lighting/PointLight.hpp"

#include "ImBridge/Parameter.hpp"

#include <functional>

namespace glrenderer
{

RendererContext::RendererContext()
{
	_bridge = std::make_shared<ImBridge::Bridge>();
	_bridge->addCombo(
		"Renderer",
		"Forward\0 Deferred\0\0",
		2,
		[this](unsigned int id) {this->SwitchRendererID(id); },
		"Change rendering algorithm.");
	_bridge->addListBox(
		"Display Viewport",
		{ "LightingTextured", "Solid", "Textured", "Wireframe" },
		[this](unsigned int id) {this->SetViewportBufferState(id); },
		"Change viewport state.");

	InitializeContext();
	SwitchRenderer(_rendererType);
}

void RendererContext::InitializeContext()
{
	//SetClearColor(glm::vec4(0.15, 0.15, 0.15, 1.0));

	//depthMaterial.setShader(std::make_shared<Shader>("res/shaders/Depth.vert", "res/shaders/Depth.frag"));

	_renderBuffer = Framebuffer::createRenderingBuffer(_width, _height); // used for main rendering (viewport)

	_shadowMap = Framebuffer::createDepthBuffer(_width, _height);     // used for shadow mapping

	// Create Point Lights UBO
	glGenBuffers(1, &_pointLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, _pointLightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLightData) * GLOBAL_MAX_LIGHTS_COUNT, NULL, GL_DYNAMIC_DRAW);
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
	SetCamera(camera);
}

void RendererContext::PostRenderContext()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RendererContext::SetCamera(const std::shared_ptr<Camera>& camera)
{
	_cameraData.viewProjectionMatrix = camera->getViewProjectionMatrix();
	_cameraData.position = camera->getPosition();
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

	_renderer->Free();
	_renderBuffer->free();
	_shadowMap->free();
	glDeleteBuffers(1, &_pointLightsUBO);
}

void RendererContext::SwitchRendererID(int inRendererTypeID)
{
	ERendererType inRendererType = static_cast<ERendererType>(inRendererTypeID);
	if (SwitchRenderer(inRendererType))
	{
		SC_SwitchRenderer(_renderer->GetMaximumLightCount());
	}
}

bool RendererContext::SwitchRenderer(ERendererType inRendererType)
{
	if (_renderer && inRendererType == _renderer->GetType())
	{
		return false;
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

				OnRendererSwitch();

				return true;
			}
		}
	}

	// Otherwise, we need to create this renderer
	CreateRenderer(inRendererType);
	if (_renderer)
	{
		_renderer->Initialize();
		_renderer->Start();
		_rendererList.push_back(_renderer);
		OnRendererSwitch();

		return true;
	}

	return false;
}

void RendererContext::OnRendererSwitch()
{
	UpdateViewportBufferList();

	// Check if lights count is above current renderer limit
	if (_pointLightsNum > _renderer->GetMaximumLightCount())
	{
		_pointLightsNum = _renderer->GetMaximumLightCount();
	}

	//glBindBuffer(GL_UNIFORM_BUFFER, _pointLightsUBO);
	//glBufferData(GL_UNIFORM_BUFFER, sizeof(Glsl_PointLight) * _renderer->GetMaximumLightCount(), NULL, GL_DYNAMIC_DRAW);
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RendererContext::UpdateViewportBufferList()
{
	if (auto parameter = _bridge->getParameter<ImBridge::ParameterListBox>("Display Viewport"))
	{
		parameter->_items.clear();
		parameter->_items.push_back("LightingTextured");
		parameter->_items.push_back("Solid");
		parameter->_items.push_back("Textured");
		parameter->_items.push_back("Wireframe");

		_renderer->AddViewportBufferInList(parameter->_items);
	}
}

void RendererContext::CreateRenderer(ERendererType rendererType)
{
	RendererData rendererData = {
		_width,
		_height,
		_renderBuffer->getId()
	};

	switch (rendererType)
	{
	case glrenderer::ERendererType::FORWARD:
	{
		_renderer = std::make_shared<ForwardRenderer>(rendererData, this);
		break;
	}
	case glrenderer::ERendererType::DEFERRED:
	{
		_renderer = std::make_shared<DeferredRenderer>(rendererData, this);
		break;
	}
	default:
	{
		std::cout << "RendererContext::CreateRenderer error : Unknown render type" << std::endl;
		return;
	}
	}
}

void RendererContext::OnLightUpdate(uint32_t lightCount, int startBytesOffset, uint32_t modifiedLightCount, PointLightData* data)
{
	if (lightCount < _pointLightsNum)
	{
		_pointLightsNum = lightCount;
		return;
	}

	if (lightCount > _renderer->GetMaximumLightCount())
	{
		return;
	}

	_pointLightsNum = lightCount;
	glBindBuffer(GL_UNIFORM_BUFFER, _pointLightsUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, (GLintptr)startBytesOffset, sizeof(PointLightData) * modifiedLightCount, data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RendererContext::SetEvents(const std::shared_ptr<Scene>& scene)
{
	auto OnSwitchRendererCallback = std::bind(&Scene::OnRendererSwitch, scene.get(), std::placeholders::_1);
	SC_SwitchRenderer = OnSwitchRendererCallback;
	SC_SwitchRenderer(_renderer->GetMaximumLightCount());
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

uint32_t RendererContext::GetRenderBufferTextureID() const
{
	return _renderBuffer->getTextureId();
}


void RendererContext::SetViewportBufferState(int bufferStateID)
{
	if (bufferStateID >= 0 && bufferStateID <= 7)
		_viewportBufferState = static_cast<EViewportBufferState>(bufferStateID);

	// update shaders if needed
	if (_viewportBufferState == EViewportBufferState::LightingTextured
		|| _viewportBufferState == EViewportBufferState::Solid
		|| _viewportBufferState == EViewportBufferState::Textured
		|| _viewportBufferState == EViewportBufferState::Wireframe)
	{
		// Update shaders

	}
}

void RendererContext::SendLightingUniforms(const std::shared_ptr<Shader>& shader)
{
	shader->SetUniform1i(GLOBAL_UNIFORM_NAME.NumPointLights, _pointLightsNum);
}

void RendererContext::SendCameraUniforms(const std::shared_ptr<Shader>& shader)
{
	shader->SetUniformMatrix4fv(GLOBAL_UNIFORM_NAME.viewProjectionMatrix, _cameraData.viewProjectionMatrix);
	shader->SetUniform3f(GLOBAL_UNIFORM_NAME.cameraPostion, _cameraData.position);
}

void RendererContext::SendShadowsUniforms(const std::shared_ptr<Shader>& shader)
{
	if (!_shadowProperties->getComputeShadows())
	{
		return;
	}

	shader->SetUniform1i("shadowMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _shadowMap->getId());

	shader->SetUniform1i("uSoftShadows", _shadowProperties->getSoftShadows() ? 1 : 0);
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

void RendererContext::SendModelUniform(const std::shared_ptr<Shader>& shader, const glm::mat4& modelMatrix)
{
	shader->SetUniformMatrix4fv(GLOBAL_UNIFORM_NAME.modelMatrix, modelMatrix);
}


}