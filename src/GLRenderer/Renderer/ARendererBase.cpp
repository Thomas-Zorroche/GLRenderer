#include "ARendererBase.hpp"

#include "RendererContext.hpp"

namespace glrenderer
{

ARendererBase::ARendererBase(RendererData rendererData, RendererContext* rendererContext)
	: _rendererData(rendererData)
{
	Cb_SendLightingUniforms = std::bind<void>(&RendererContext::SendLightingUniforms, rendererContext, std::placeholders::_1);
	Cb_SendCameraUniforms = std::bind<void>(&RendererContext::SendCameraUniforms, rendererContext, std::placeholders::_1);
	Cb_SendShadowsUniforms = std::bind<void>(&RendererContext::SendShadowsUniforms, rendererContext, std::placeholders::_1);
	Cb_SendModelUniform = std::bind<void>(&RendererContext::SendModelUniform, rendererContext, std::placeholders::_1, std::placeholders::_2);
}

void ARendererBase::SendLightingUniforms(const std::shared_ptr<Shader>& shader)
{
	Cb_SendLightingUniforms(shader);
}

void ARendererBase::SendCameraUniforms(const std::shared_ptr<Shader>& shader)
{
	Cb_SendCameraUniforms(shader);
}

void ARendererBase::SendShadowsUniforms(const std::shared_ptr<Shader>& shader)
{
	Cb_SendShadowsUniforms(shader);
}

void ARendererBase::SendModelUniform(const std::shared_ptr<Shader>& shader, const glm::mat4& modelMatrix)
{
	Cb_SendModelUniform(shader, modelMatrix);
}

}