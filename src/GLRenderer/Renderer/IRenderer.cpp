#include "IRenderer.hpp"

#include "glad/glad.h"



namespace glrenderer
{

void IRenderer::RenderScene(const std::shared_ptr<Camera>& camera, entt::registry& scene, const Entity& entitySelected)
{
	PreRender(camera);

	Render(scene, entitySelected);

	PostRender();
}

void IRenderer::Clear()
{
	glStencilMask(0xFF);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

// **********************************************************************
// Draw functions
// **********************************************************************
void IRenderer::DrawVAO(const std::shared_ptr<VertexArray>& vertexArray)
{
	vertexArray->bind();

	if (vertexArray->getIndexBuffer())
	{
		glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, vertexArray->getVertexCount());
	}
}

void IRenderer::DrawLine(const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform, bool selected)
{
	// TODO move material and color constant to SCENE
	// renderer should only receive VAO, Transform and shader
	//auto& shader = flatMaterial.getShader();
	//shader->Bind();
	//shader->SetUniformMatrix4fv("uProjectionMatrix", _cameraData.viewProjectionMatrix);
	//shader->SetUniformMatrix4fv("uModelMatrix", transform);
	//shader->SetUniform3f("uColor", selected ? glm::vec3(0.0f, 0.95f, 0.40f) : glm::vec3(0.0f, 0.0f, 0.0f));
	//
	//glStencilMask(0x00);
	//vertexArray->bind();
	//glDrawArrays(GL_LINES, 0, 2);
}


//glm::mat4 Scene::getLightSpaceMatrix()
//{
//	// TODO member dirLight
//	glm::vec3 lightDirection;
//	glm::mat4 rot;
//	float offset = 0.0f;
//
//	float far_plane = 0.0f;
//	float near_plane = 0.0f;
//
//	auto view = _registry.view<LightComponent>();
//	for (auto& entityId : view)
//	{
//		Entity entity = { entityId, this };
//		auto light = entity.getComponent<LightComponent>().light;
//		DirectionalLight* dirLight = light->isDirectionalLight();
//		if (dirLight)
//		{
//			far_plane = dirLight->getFarPlane();
//			near_plane = dirLight->getNearPlane();
//			offset = dirLight->getOffsetPosition();
//			lightDirection = entity.getComponent<LineComponent>().line->getDirection();
//
//			auto rotation = entity.getComponent<TransformComponent>().rotation;
//			// TODO optimize this
//			rot = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));
//			rot = glm::rotate(rot, glm::radians(rotation.y), glm::vec3(0, 1, 0));
//			rot = glm::rotate(rot, glm::radians(rotation.x), glm::vec3(1, 0, 0));
//
//			break;
//		}
//	}
//
//	glm::mat4 lightView = glm::lookAt(
//		glm::vec3(rot * glm::vec4(lightDirection, 1.0)) * glm::vec3(offset), // position
//		glm::vec3(rot * glm::vec4(lightDirection, 1.0)),   // target
//		glm::vec3(0.0f, 1.0f, 0.0f)    // up vector
//	);
//	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
//
//	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
//
//	return lightSpaceMatrix;
//}

}