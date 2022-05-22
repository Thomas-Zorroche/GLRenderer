#pragma once

#include "IRenderer.hpp"

#include "../Shader.h"

namespace glrenderer
{

class DeferredRenderer : public IRenderer
{
public:
	DeferredRenderer() = default;

public:
	// Mandatory override functions
	void Render(entt::registry& scene, const Entity& entitySelected) override;

	uint32_t GetMaximumLightCount() const override { return _maximumLightCount; }

	virtual ERendererType GetType() const override { return ERendererType::DEFERRED; }

	virtual void Free() override;
// End of mandatory override functions

public:
	virtual void Initialize(const SendGlobalUniformCallback& SendGlobalUniformCb, uint32_t width, uint32_t height) override;

	virtual void PreRender(const std::shared_ptr<Camera>& camera) override;

	virtual void PostRender() override;

private:
	void createBuffers(unsigned int width, unsigned int height);

	void geometryPass();

	void lightingPass();

	void renderQuad();

protected:
	uint32_t _maximumLightCount = 200;

	// TODO inside abstract class ?
protected:
	SendGlobalUniformCallback SendGlobalUniform;

private:
	unsigned int _gBuffer;

	unsigned int _gPosition;
	unsigned int _gNormal;
	unsigned int _gColorSpec;

	unsigned int _quadVAO = 0;

	Shader _geometryPassShader = Shader("res/shaders/deferredRendering/geometry_pass.vs", "res/shaders/deferredRendering/geometry_pass.fs");
	Shader _lightingPassShader = Shader("res/shaders/deferredRendering/lighting_pass.vs", "res/shaders/deferredRendering/lighting_pass.fs");
};

}