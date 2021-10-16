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

	void Scene::onUpdate(const glm::mat4& projectionMatrix)
	{
		auto group = _registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group)
		{
			auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

			auto& material = mesh.mesh->getMaterial();
			material->bind();
			{
				material->setModelMatrix(transform.getTransformMatrix());
				material->setProjectionMatrix(projectionMatrix);
			}

			Renderer::draw(mesh.mesh->getVertexArray());
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