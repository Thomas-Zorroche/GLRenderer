#pragma once

#include "IRenderer.hpp"



namespace glrenderer
{

class ForwardRenderer : public IRenderer
{
public:
	ForwardRenderer() = default;

public:
// Mandatory override functions
	void Render(entt::registry& scene, const Entity& entitySelected) override;

	uint32_t GetMaximumLightCount() const override { return _maximumLightCount; }

	virtual ERendererType GetType() const override { return ERendererType::FORWARD; }

	virtual void Free() override;
// End of mandatory override functions

public:
	virtual void Initialize(const SendGlobalUniformCallback& SendGlobalUniformCb, uint32_t width, uint32_t height) override;
	
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