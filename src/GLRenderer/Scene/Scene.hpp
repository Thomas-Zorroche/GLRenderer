#pragma once

#include "entt/entt.hpp"
#include <string>

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

	private:
		friend class Entity;
	private:
		entt::registry _registry;
	};
}