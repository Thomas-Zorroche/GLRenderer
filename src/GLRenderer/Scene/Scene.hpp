#pragma once

#include "entt/entt.hpp"
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "../Shader.h"
#include "../Lighting/DirectionalLight.hpp"
#include "../Lighting/PointLight.hpp"


namespace glrenderer {

	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void renderScene(const std::shared_ptr<class Camera>& camera, const Entity& entitySelected);

		void computeDepthPass();
		void computeRenderPass(const std::shared_ptr<class Camera>& camera, const Entity& entitySelected);

		bool importModel(const std::string& modelPath);

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

		void setDirectionalLight(const std::shared_ptr<DirectionalLight>& dirLight)
		{
			_directionalLight = dirLight;
		}
		const std::shared_ptr<DirectionalLight>& getDirectionalLight() const { return _directionalLight; }
		std::shared_ptr<DirectionalLight> getDirectionalLight() { return _directionalLight; }

		// TEMP
		glm::mat4 getLightSpaceMatrix();
		void createLights(size_t numLights);
		void updatePointLights(size_t pointLightsNum);

		size_t getPointLightNum() const { return _pointLights.size(); }


	private:
		bool isLabelDuplicate(const std::string& name);
		
		// TODO: Move this outside Scene class
		void sendLightingUniforms(const std::shared_ptr<Shader>& shader);

		friend class Entity;

	private:
		entt::registry _registry;
		std::shared_ptr<DirectionalLight> _directionalLight = nullptr;
		std::vector<Glsl_PointLight> _pointLights = std::vector<Glsl_PointLight>();
	};
}