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
		auto& label = entity.addComponent<LabelComponent>();
		label.label = name == "" ? "Entity" : name;

		entity.addComponent<TransformComponent>();
		return entity;
	}

}