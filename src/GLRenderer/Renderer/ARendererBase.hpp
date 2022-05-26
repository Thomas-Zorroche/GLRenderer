#pragma once
#include <cstdint>
#include <memory>

#include "IRenderer.hpp"

namespace glrenderer
{

struct RendererData
{
// References
	uint32_t& _renderBufferWidth;
	uint32_t& _renderBufferHeight;

// Constants
	const int RENDER_BUFFER;
};


class ARendererBase : public IRenderer
{
public:
	ARendererBase(RendererData rendererData, class RendererContext* rendererContext);

protected:
// Send uniforms callbacks
	void SendLightingUniforms(const std::shared_ptr<Shader>& shader);
	void SendCameraUniforms(const std::shared_ptr<Shader>& shader);
	void SendShadowsUniforms(const std::shared_ptr<Shader>& shader);
	void SendModelUniform(const std::shared_ptr<Shader>& shader, const glm::mat4& modelMatrix);
// end of send uniforms callbacks

protected:
	RendererData _rendererData;

	std::shared_ptr<Material> _flatMaterial = nullptr;

private:
// Callbacks
	using CbType_SendUniforms = std::function<void(const std::shared_ptr<Shader>&)>;
	using CbType_SendModelUniform = std::function<void(const std::shared_ptr<Shader>&, const glm::mat4&)>;

	CbType_SendUniforms Cb_SendLightingUniforms;
	CbType_SendUniforms Cb_SendCameraUniforms;
	CbType_SendUniforms Cb_SendShadowsUniforms;
	CbType_SendModelUniform Cb_SendModelUniform;
// end of callbacks
};

}
