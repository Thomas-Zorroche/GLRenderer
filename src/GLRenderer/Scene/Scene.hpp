﻿#pragma once

#include "entt/entt.hpp"
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace glrenderer {

	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void onUpdate(const glm::mat4& projectionMatrix); // TEMP proj matrix

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

		void makeUniqueLabel(std::string& label);

	private:
		bool isLabelDuplicate(const std::string& name);

		friend class Entity;

	private:
		entt::registry _registry;
	};
}