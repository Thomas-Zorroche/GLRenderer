#include "Scene.hpp"
#include "Component.hpp"
#include "../Renderer.hpp"

#include "Entity.hpp"

// TEMP
#include "GLRenderer/Lighting/PointLight.hpp"

namespace glrenderer {

	Scene::Scene()
	{

	}

	Scene::~Scene()
	{

	}

	void Scene::onUpdate()
	{
		auto group = _registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group)
		{
			auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			auto& shader = mesh.mesh->getMaterial()->getShader();

			shader->Bind();
			sendLightingUniforms(shader);
			
			Renderer::draw(mesh.mesh->getVertexArray(), shader, transform.getModelMatrix());
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

		for (auto entityLight : viewLight)
		{
			Entity entity = { entityLight, this };
			std::shared_ptr<BaseLight>& baseLight = entity.getComponent<LightComponent>().light;

			// Common light properties
			shader->SetUniform3f("pointLights[" + lightIndexStr + "].diffuse", baseLight->getColor());
			shader->SetUniform3f("pointLights[" + lightIndexStr + "].ambient", { 1.0, 1.0, 1.0 });
			shader->SetUniform3f("pointLights[" + lightIndexStr + "].specular", { 1.0, 1.0, 1.0 });
			shader->SetUniform1f("pointLights[" + lightIndexStr + "].intensity", baseLight->getIntensity());

			// Special light properties
			PointLight* light = baseLight->isPointLight();
			if (light)
			{
				glm::vec3& position = entity.getComponent<TransformComponent>().location;
				shader->SetUniform3f("pointLights[" + lightIndexStr + "].position", position);
				shader->SetUniform1f("pointLights[" + lightIndexStr + "].linear", 0.07);
				shader->SetUniform1f("pointLights[" + lightIndexStr + "].quadratic", 0.017);
			}

			lightIndex++;
			lightIndexStr = std::to_string(lightIndex);
		}
	}

}