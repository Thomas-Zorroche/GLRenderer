#pragma once

#include "ARendererBase.hpp"

#include <memory>

#include "../Camera.hpp"
#include "../Scene/Entity.hpp"

#include "ImBridge/Bridge.hpp"

namespace glrenderer
{

/*
* RendererContext class that provides essential data for all renderers
* 
* represents the global state (context) of rendering
* (functions here are called for each renderer)
* 
* this class (therefore this data) is persistent throughout the program
*/
class RendererContext
{
public:
	RendererContext();

	void RenderScene(const std::shared_ptr<Camera>& camera, entt::registry& scene, const Entity& entitySelected);
	
	void Free();

	void Resize(uint32_t width, uint32_t height);

	const std::shared_ptr<ARendererBase>& GetRenderer() const { return _renderer; }

	void OnLightUpdate(const std::vector<Glsl_PointLight>& lights);

	const std::shared_ptr<ShadowsProperties>& GetShadowProperties() const {
		return _shadowProperties;
	}

	const std::shared_ptr<ImBridge::Bridge>& GetBridge() { return _bridge; }

	void SetEvents(const std::shared_ptr<Scene>& scene);

	uint32_t GetRenderBufferTextureID() const;

public:
// Renderer callbacks
	void SendLightingUniforms(const std::shared_ptr<Shader>& shader);
	void SendCameraUniforms(const std::shared_ptr<Shader>& shader);
	void SendShadowsUniforms(const std::shared_ptr<Shader>& shader);
	void SendModelUniform(const std::shared_ptr<Shader>& shader, const glm::mat4& modelMatrix);
// end of renderer callbacks

private:
	void InitializeContext();

	void PreRenderContext(const std::shared_ptr<Camera>& camera);

	void PostRenderContext();

	void SetCamera(const std::shared_ptr<Camera>& camera);

	void CreateRenderer(ERendererType rendererType);

	void SetClearColor(const glm::vec3& color);

	void SwitchRendererID(int inRendererTypeID);
	bool SwitchRenderer(ERendererType inRendererType);

	void SetViewportBufferState(int bufferStateID);

	void UpdateViewportBufferList();

	void OnRendererSwitch();

// Events
public:
	using OnSwitchRendererCallback = std::function<void(int newMaxRendererLights)>;

private:
	OnSwitchRendererCallback SC_SwitchRenderer;
// End of events

private:
// ARendererBase data
	// Render framebuffer size
	uint32_t _width = 1280;
	uint32_t _height = 720;

	int _pointLightsNum = 0;

// End of ARendererBase data

private:
	std::shared_ptr<ARendererBase> _renderer = nullptr;

	std::vector<std::shared_ptr<glrenderer::ARendererBase>> _rendererList;
	
	struct CameraData
	{
		glm::vec3 position;
		glm::mat4 viewProjectionMatrix;
	};
	CameraData _cameraData;

	struct GlobalUniformName
	{
		std::string modelMatrix = "uModelMatrix";
		std::string viewProjectionMatrix = "uProjectionMatrix";
		std::string cameraPostion = "uCameraPos";
		std::string pointLights = "uPointLights";
		std::string NumPointLights = "uNumPointLights";

	};
	GlobalUniformName GLOBAL_UNIFORM_NAME;

	std::shared_ptr<ShadowsProperties> _shadowProperties = std::make_shared<ShadowsProperties>();

	std::unique_ptr<class Framebuffer> _shadowMap = nullptr;
	std::unique_ptr<class Framebuffer> _renderBuffer = nullptr;

	// TEMP
	Material flatMaterial = Material(nullptr);
	//Material depthMaterial;
	unsigned int _pointLightsUBO = 0;

	ERendererType _rendererType = ERendererType::FORWARD;

	glm::vec3 _clearColor = glm::vec3(0.15f, 0.15f, 0.15f);

	EViewportBufferState _viewportBufferState = EViewportBufferState::LightingTextured;

	std::shared_ptr<ImBridge::Bridge> _bridge;
};

}