#include "Scene.hpp"
#include "Component.hpp"
#include "../Renderer.hpp"

#include "Entity.hpp"

#include "glm/gtc/matrix_transform.hpp"

// TEMP
#include "GLRenderer/Lighting/PointLight.hpp"
#include "GLRenderer/Lighting/DirectionalLight.hpp"

namespace glrenderer {

	glm::mat4 Scene::getLightSpaceMatrix()
	{
		// TODO member dirLight
		glm::vec3 lightDirection;
		glm::mat4 rot;
		float offset = 0.0f;

		float far_plane = 0.0f;
		float near_plane = 0.0f;

		auto view = _registry.view<LightComponent>();
		for (auto& entityId : view)
		{
			Entity entity = { entityId, this };
			auto light = entity.getComponent<LightComponent>().light;
			DirectionalLight* dirLight = light->isDirectionalLight();
			if (dirLight)
			{
				far_plane = dirLight->getFarPlane();
				near_plane = dirLight->getNearPlane();
				offset = dirLight->getOffsetPosition();
				lightDirection = entity.getComponent<LineComponent>().line->getDirection();

				auto rotation = entity.getComponent<TransformComponent>().rotation;
				// TODO optimize this
				rot = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));
				rot = glm::rotate(rot, glm::radians(rotation.y), glm::vec3(0, 1, 0));
				rot = glm::rotate(rot, glm::radians(rotation.x), glm::vec3(1, 0, 0));

				break;
			}
		}

		glm::mat4 lightView = glm::lookAt(
			glm::vec3(rot * glm::vec4(lightDirection, 1.0)) * glm::vec3(offset), // position
			glm::vec3(rot * glm::vec4(lightDirection, 1.0)),   // target
			glm::vec3(0.0f, 1.0f, 0.0f)    // up vector
		);
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		
		return lightSpaceMatrix;
	}

	Scene::Scene()
	{

	}

	Scene::~Scene()
	{

	}

	void Scene::onUpdate(const Entity& entitySelected, bool depth, unsigned int depthId)
	{
		// Render Mesh
		auto group = _registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (const auto entity : group)
		{
			auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

			// Depth rendering (shadow mapping)
			if (depth)
			{
				Renderer::drawDepth(mesh.mesh->getVertexArray(), transform.getModelMatrix(),
					getLightSpaceMatrix());
				continue;
			}

			// Main rendering
			auto& shader = mesh.mesh->getMaterial()->getShader();
			shader->Bind();

			// TODO Optimize this
			glm::mat4 lightSpaceMatrix = getLightSpaceMatrix();
			shader->SetUniformMatrix4fv("uLightSpaceMatrix", lightSpaceMatrix);

			sendLightingUniforms(shader);
			Renderer::draw(mesh.mesh->getVertexArray(), shader, transform.getModelMatrix(), 
				entitySelected == entity, depthId);
		}

		// Render Lines
		if (!depth)
		{
			auto group2 = _registry.group<LineComponent>(entt::get<TransformComponent>);
			for (const auto entity : group2)
			{
				auto [transform, line] = group2.get<TransformComponent, LineComponent>(entity);

				Renderer::drawLine(line.line->getVertexArray(), transform.getModelMatrix(), entitySelected == entity);
			}
		}
	}

	Entity Scene::createEntity(const std::string& name)
	{
		Entity entity = { _registry.create(), this };
		auto& labelEntity = entity.addComponent<LabelComponent>();
		labelEntity.label = (name == "") ? "Entity" : name;

		// Avoid duplicate labels
		// TODO: Optimize this
		if (isLabelDuplicate(labelEntity.label))
			makeUniqueLabel(labelEntity.label);

		entity.addComponent<TransformComponent>();
		return entity;
	}

	void Scene::makeUniqueLabel(std::string& label)
	{
		if (!isLabelDuplicate(label))
			return;

		uint32_t index = 1;
		std::string baseLabel = label;
		do
		{
			label = baseLabel + std::to_string(index);
			index++;
		} while (isLabelDuplicate(label));
	}

	bool Scene::isLabelDuplicate(const std::string& label)
	{
		auto view = _registry.view<LabelComponent>();
		int duplicate = 0;
		for (auto& entityId : view)
		{
			Entity entity = { entityId, this };
			std::string& labelOther = entity.getComponent<LabelComponent>().label;

			if (label == labelOther)
			{
				duplicate++;
				if (duplicate >= 2)
					return true;
			}
		}
		return false;
	}

	void Scene::sendLightingUniforms(const std::shared_ptr<Shader>& shader)
	{
		auto viewLight = _registry.view<LightComponent>();
		uint32_t lightIndex = 0;
		std::string lightIndexStr = std::to_string(lightIndex);

		// TEMP
		// Only one directional light can be used
		bool dirLightUsed = false;

		for (auto entityLight : viewLight)
		{
			Entity entity = { entityLight, this };
			std::shared_ptr<BaseLight>& baseLight = entity.getComponent<LightComponent>().light;

			PointLight* pointLight = baseLight->isPointLight();
			if (pointLight)
			{
				shader->SetUniform3f("pointLights[" + lightIndexStr + "].diffuse", baseLight->getColor());
				shader->SetUniform3f("pointLights[" + lightIndexStr + "].ambient", baseLight->getColor());
				shader->SetUniform3f("pointLights[" + lightIndexStr + "].specular", { 1.0, 1.0, 1.0 });
				shader->SetUniform1f("pointLights[" + lightIndexStr + "].intensity", baseLight->getIntensity());

				glm::vec3& position = entity.getComponent<TransformComponent>().location;
				shader->SetUniform3f("pointLights[" + lightIndexStr + "].position", position);
				shader->SetUniform1f("pointLights[" + lightIndexStr + "].linear", pointLight->getLinear());
				shader->SetUniform1f("pointLights[" + lightIndexStr + "].quadratic", pointLight->getQuadratic());
				continue;
			}

			DirectionalLight* dirLight = baseLight->isDirectionalLight();
			if (dirLight && !dirLightUsed)
			{
				dirLightUsed = true;

				shader->SetUniform3f("directionalLight.diffuse", baseLight->getColor());
				shader->SetUniform3f("directionalLight.ambient", { 0.5, 0.5, 0.5 });
				shader->SetUniform3f("directionalLight.specular", { 1.0, 1.0, 1.0 });
				shader->SetUniform1f("directionalLight.intensity", baseLight->getIntensity());
				shader->SetUniform1i("directionalLight.softShadow", _directionalLight->getSoftShadow());
				shader->SetUniform1f("directionalLight.size", _directionalLight->getSize());
				float frustumSize = 2 * _directionalLight->getNearPlane() * std::tanf(60.0f * 0.5f) * 1.0f;
				shader->SetUniform1f("directionalLight.frustumSize", _directionalLight->getFrustumSize());
				shader->SetUniform1f("directionalLight.nearPlane", _directionalLight->getNearPlane());

				auto& line = entity.getComponent<LineComponent>().line;
				auto rotation = entity.getComponent<TransformComponent>().rotation;
				// TODO optimize this
				glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));
				rot = glm::rotate(rot, glm::radians(rotation.y), glm::vec3(0, 1, 0));
				rot = glm::rotate(rot, glm::radians(rotation.x), glm::vec3(1, 0, 0));

				shader->SetUniform3f("directionalLight.direction", rot * glm::vec4(line->getDirection(), 1.0f));
				continue;
			}

			lightIndex++;
			lightIndexStr = std::to_string(lightIndex);
		}
	}

}