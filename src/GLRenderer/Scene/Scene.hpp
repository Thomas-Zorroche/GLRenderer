#pragma once

#include "entt/entt.hpp"
#include <string>
#include <vector>

// TEMP
#include "../Shader.h"

namespace glrenderer {

	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void onUpdate(std::shared_ptr<Shader>& shader);

		Entity createEntity(const std::string& label = "");

		template <typename Function>
		void forEachEntity(Function function)
		{
			auto view = _registry.view<TransformComponent>();
			for (const auto& entityId : view)
			{
				Entity entity = { entityId, this };
				function(entity);
			}
		}

	private:
		friend class Entity;
	private:
		entt::registry _registry;
	};
}