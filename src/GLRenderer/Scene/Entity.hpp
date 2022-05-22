#pragma once

#include <iostream>
#include <string>

#include <entt/entt.hpp>
#include "Scene.hpp"

namespace glrenderer {

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity id, Scene* scene);
		Entity(const Entity&) = default;

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

		bool operator==(const Entity& rhs) { return _entityId == rhs._entityId; }
		bool operator==(const entt::entity rhs) const { return _entityId == rhs; }

	private:
		Scene* _scene = nullptr;
		entt::entity _entityId = entt::null;
	};


}


