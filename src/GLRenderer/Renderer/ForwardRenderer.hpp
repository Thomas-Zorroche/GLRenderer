#pragma once

#include "ARendererBase.hpp"



namespace glrenderer
{

class ForwardRenderer : public ARendererBase
{
public:
	ForwardRenderer(RendererData rendererData, class RendererContext* renderContext);

public:
// Mandatory override functions
	void Render(entt::registry& scene, const Entity& entitySelected) override;

	uint32_t GetMaximumLightCount() const override { return _maximumLightCount; }

	virtual ERendererType GetType() const override { return ERendererType::FORWARD; }

	virtual void Free() override;
// End of mandatory override functions

public:
	virtual void Initialize() override;
	
protected:
	void DrawRenderPass(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader,
		const glm::mat4& transform, bool selected);

	void DrawDepthPass();

protected:
	uint32_t _maximumLightCount = 200;

// TODO inside abstract class ?
protected:
	SendGlobalUniformCallback SendGlobalUniform;
};

}