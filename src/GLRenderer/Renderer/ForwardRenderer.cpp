#include "ForwardRenderer.hpp"

#include "glad/glad.h"

#include "../Scene/Component.hpp"
#include "../Framebuffer.hpp"

namespace glrenderer
{

ForwardRenderer::ForwardRenderer(RendererData rendererData, RendererContext* renderContext)
	: ARendererBase(rendererData, renderContext)
{

}


void ForwardRenderer::Initialize()
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

	// Clear Color
	glClearColor(0.141f, 0.207f, 0.211f, 1.0f);
}

void ForwardRenderer::Render(entt::registry& scene, const Entity& entitySelected)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _rendererData.RENDER_BUFFER);

	glStencilMask(0xFF);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Depth Pass
	//Renderer::getShadowMap()->bind(); // TODO replace 1024 by variable
	//Renderer::clear();
	//
	//auto group = _registry.group<TransformComponent>(entt::get<MeshComponent>);
	//for (const auto entity : group)
	//{
	//	auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
	//
	//	// Depth rendering (shadow mapping)
	//	Renderer::drawDepth(mesh.mesh->getVertexArray(), transform.getModelMatrix(), getLightSpaceMatrix());
	//}
	//
	//Renderer::getShadowMap()->unbind();
	 
	
	// Render Meshes
	auto group = scene.group<TransformComponent>(entt::get<MeshComponent>);
	for (const auto entity : group)
	{
		auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

		const std::shared_ptr<Material>& material = mesh.mesh->getMaterial();
		const std::shared_ptr<Shader>& shader = material->getShader();
		material->bind();

		SendCameraUniforms(shader);
		SendShadowsUniforms(shader);
		SendLightingUniforms(shader);
		SendModelUniform(shader, transform.getModelMatrix());

		// TODO Optimize this
		//if (_shadowProperties->getComputeShadows())
		//{
		//	glm::mat4 lightSpaceMatrix = getLightSpaceMatrix();
		//	shader->SetUniformMatrix4fv("uLightSpaceMatrix", lightSpaceMatrix);
		//}

		DrawRenderPass(mesh.mesh->getVertexArray(), shader, transform.getModelMatrix(),
			entitySelected == entity);
	}

	// Render Lines
	//auto group2 = scene.group<LineComponent>(entt::get<TransformComponent>);
	//for (const auto entity : group2)
	//{
	//	auto [transform, line] = group2.get<TransformComponent, LineComponent>(entity);
	//
	//	DrawLine(line.line->getVertexArray(), transform.getModelMatrix(), entitySelected == entity);
	//}

}

void ForwardRenderer::DrawRenderPass(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader,
	const glm::mat4& transform, bool selected)
{
	if (selected)
	{
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	}
	else
	{
		glStencilMask(0x00);
	}

	DrawVAO(vertexArray);

	if (selected)
	{
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		//glDisable(GL_DEPTH_TEST);
	
		// Draw selected entity with x1.1 scale
		auto& entitySelectedShader = _flatMaterial->getShader();
		entitySelectedShader->Bind();
		SendCameraUniforms(entitySelectedShader);
		SendModelUniform(entitySelectedShader, glm::scale(transform, glm::vec3(1.02f)));
		entitySelectedShader->SetUniform3f("uColor", glm::vec3(0.0f, 0.95f, 0.40f));
	
		DrawVAO(vertexArray);
	
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		//glEnable(GL_DEPTH_TEST);
	}
}

void ForwardRenderer::DrawDepthPass()
{
	//auto& shader = depthMaterial.getShader();
	//shader->Bind();
	//shader->SetUniformMatrix4fv("uModelMatrix", transform);
	//shader->SetUniformMatrix4fv("uProjectionMatrix", lightMatrix);
	//glStencilMask(0x00);
	//
	////vertexArray->bind();
	//
	////glEnable(GL_CULL_FACE);
	////glCullFace(GL_FRONT);
	//drawVAO(vertexArray);
	////glCullFace(GL_BACK); 
	////glDisable(GL_CULL_FACE);
}

void ForwardRenderer::Free()
{

}

}