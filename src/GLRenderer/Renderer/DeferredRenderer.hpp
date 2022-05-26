#pragma once

#include "ARendererBase.hpp"

#include "../Shader.h"

#include "ImBridge/Bridge.hpp"


namespace glrenderer
{


class DeferredRenderer : public ARendererBase
{
public:
	DeferredRenderer(RendererData rendererData, class RendererContext* renderContext);


public:
	// Mandatory override functions
	void Render(entt::registry& scene, const Entity& entitySelected) override;

	uint32_t GetMaximumLightCount() const override { return MAXIMUM_LIGHT_COUNT; }

	virtual ERendererType GetType() const override { return ERendererType::DEFERRED; }

	virtual void Free() override;
// End of mandatory override functions

public:
	virtual void Initialize() override;

	virtual void PreRender(const std::shared_ptr<Camera>& camera) override;

	virtual void PostRender() override;

	virtual unsigned int GetBufferTextureId(EViewportBufferState Buffer) override;
	
	virtual void AddViewportBufferInList(std::vector<std::string>& buffers) override;

	virtual void Resize(uint32_t width, uint32_t height) override;

private:
	void createBuffers(unsigned int width, unsigned int height);

	void geometryPass(entt::registry& scene);

	void lightingPass();

	void SwitchViewportBuffer(int bufferID);

private:
	const uint32_t MAXIMUM_LIGHT_COUNT = 500;

private:
	unsigned int _gBuffer = 0;

	unsigned int _gPosition = 0;
	unsigned int _gNormal = 0;
	unsigned int _gColorSpec = 0;

	unsigned int _quadVAO = 0;
	unsigned int _quadVBO = 0;

	std::shared_ptr<Shader> _geometryPassShader = nullptr;
	std::shared_ptr<Shader> _lightingPassShader = nullptr;

	std::shared_ptr<ImBridge::Bridge> _bridge = nullptr;
};

}