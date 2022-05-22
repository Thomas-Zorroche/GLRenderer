#pragma once

#include "IRenderer.hpp"

#include <memory>

#include "../Camera.hpp"
#include "../Scene/Entity.hpp"


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

	void SwitchRenderer(ERendererType inRendererType);

	void Resize(uint32_t width, uint32_t height);

	const std::shared_ptr<IRenderer>& GetRenderer() const { return _renderer; }

	void OnLightUpdate(const std::vector<Glsl_PointLight>& lights);

	unsigned int GetRenderBuffer() const;

	const std::shared_ptr<ShadowsProperties>& GetShadowProperties() const {
		return _shadowProperties;
	}

private:
	void InitializeContext();

	void PreRenderContext(const std::shared_ptr<Camera>& camera);

	void PostRenderContext();

	void SetCamera(const std::shared_ptr<Camera>& camera);

	void CreateRenderer(ERendererType rendererType);

	void SendGlobalUniform(const std::shared_ptr<Shader>& shader);


	void SetClearColor(const glm::vec3& color);

private:
	std::shared_ptr<IRenderer> _renderer = nullptr;

	std::vector<std::shared_ptr<glrenderer::IRenderer>> _rendererList;
	
	// Render framebuffer size
	uint32_t _width;
	uint32_t _height;

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

	std::unique_ptr<class Framebuffer> _shadowMap;
	std::unique_ptr<class Framebuffer> _renderBuffer;

	// TEMP
	Material flatMaterial = Material(nullptr);
	//Material depthMaterial;
	unsigned int _pointLightsUBO = 0;

	ERendererType _rendererType = ERendererType::FORWARD;

	int _pointLightsNum = 0;

	glm::vec3 _clearColor = glm::vec3(0.15f, 0.15f, 0.15f);
};

}