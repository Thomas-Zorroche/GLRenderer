#pragma once

#include "entt/entt.hpp"
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "../Shader.h"

namespace glrenderer {

	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void onUpdate();

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
		
		// TODO: Move this outside Scene class
		void sendLightingUniforms(const std::shared_ptr<Shader>& shader);

		friend class Entity;

	private:
		entt::registry _registry;
	};
}