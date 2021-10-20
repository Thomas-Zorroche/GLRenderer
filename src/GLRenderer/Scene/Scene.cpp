#include "Scene.hpp"
#include "Component.hpp"
#include "../Renderer.hpp"

#include "Entity.hpp"

namespace glrenderer {

	Scene::Scene()
	{

	}

	Scene::~Scene()
	{

	}

	void Scene::onUpdate()
	{
		// Retrive lights TEMP
		//auto viewLight = _registry.group<LightComponent>();
		//for (auto entityLight : viewLight)
		//{
		//	entityLight
		//}


		auto group = _registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group)
		{
			auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			auto& shader = mesh.mesh->getMaterial()->getShader();

			shader->Bind();

			shader->SetUniform3f("pointLight.diffuse", { 1.0, 0.0, 0.0 });
			shader->SetUniform3f("pointLight.ambient", { 1.0, 1.0, 1.0 });
			shader->SetUniform3f("pointLight.specular", { 1.0, 1.0, 1.0 });
			shader->SetUniform3f("pointLight.position", { 4.0, 5.0, 5.0 });
			shader->SetUniform1f("pointLight.linear",   0.07);
			shader->SetUniform1f("pointLight.quadratic", 0.017);
			shader->SetUniform1f("pointLight.intensity", 1.0f);

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
}