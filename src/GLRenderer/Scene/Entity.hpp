#pragma once

#include "Scene.hpp"

#include <entt/entt.hpp>
#include <string>
#include <iostream>

namespace glrenderer {

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity id, Scene* scene, const std::string& label = "Entity");
		Entity(const Entity& other) = default;

		~Entity();

		template<typename T>
		T* hasComponent()
		{
			return _scene->_registry.try_get<T>(_entityId);
		}

		template<typename T>
		T& getComponent()
		{
			if (!hasComponent<T>())
				std::cerr << "Entity does not have this type of component." << std::endl;

			return _scene->_registry.get<T>(_entityId);
		}

		template<typename T, typename... Args>
		T& addComponent(Args&&... components)
		{
			if (hasComponent<T>())
				std::cerr << "Entity already has this type of component." << std::endl;

			T& component = _scene->_registry.emplace<T>(_entityId, components...);
			return component;
		}

		operator bool() const { return _scene && _entityId != entt::null; }

	private:
		Scene* _scene = nullptr;
		entt::entity _entityId = entt::null;
		std::string _label;
	};
}