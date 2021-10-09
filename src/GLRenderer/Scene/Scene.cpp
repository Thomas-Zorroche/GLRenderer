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

	void Scene::onUpdate(std::shared_ptr<Shader>& shader)
	{

		auto group = _registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group)
		{
			auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
		
			shader->SetUniformMatrix4fv("uModelMatrix", transform.getTransformMatrix());
			Renderer::draw(mesh.mesh->getVertexArray(), transform.getTransformMatrix());
		}
	}

	Entity Scene::createEntity(const std::string& label)
	{
		Entity entity = { _registry.create(), this, label };
		entity.addComponent<TransformComponent>();
		return entity;
	}


}