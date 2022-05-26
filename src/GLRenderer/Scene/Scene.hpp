#pragma once

#include "entt/entt.hpp"
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "../Shader.h"
#include "../Lighting/DirectionalLight.hpp"
#include "../Lighting/PointLight.hpp"

#include "ImBridge/Bridge.hpp"


namespace glrenderer {

enum class EBaseEntityType
{
	Cube = 0,
	Plan,
	Sphere,

	PointLight,
	DirectionalLight,
	SpotLight,
	
	ParticuleSystem
};

class Entity;

class Scene
{
public:
	Scene(const std::shared_ptr<class RendererContext>& rendererContext);
	~Scene();

	const std::shared_ptr<ImBridge::Bridge>& GetBridge() { return _bridge; }

	friend class Entity;

public:
// Entity Management
	bool ImportModel(const std::string& modelPath, const uint32_t& meshGroupId);

	void CreateBaseEntity(EBaseEntityType baseEntityType);

	void RenameEntity(Entity& entity, const std::string& name);

	entt::registry& GetScene() { return _registry; }

	template <typename Function>
	void forEachEntity(Function function)
	{
		auto view = _registry.view<TransformComponent>();
		for (const auto& entityId : view)
		{
			function(Entity(entityId, this));
		}
	}

private:
	Entity CreateEntity(const std::string& label = "");

	void makeUniqueLabel(std::string& label);

	bool isLabelDuplicate(const std::string& name);

// End of entity management

	void setDirectionalLight(const std::shared_ptr<DirectionalLight>& dirLight)
	{
		_directionalLight = dirLight;
	}
	const std::shared_ptr<DirectionalLight>& getDirectionalLight() const { return _directionalLight; }
	std::shared_ptr<DirectionalLight> getDirectionalLight() { return _directionalLight; }

	// TEMP
	//glm::mat4 getLightSpaceMatrix();
	void createLights(size_t numLights);
	void updatePointLights(size_t pointLightsNum);

	size_t getPointLightNum() const { return _pointLights.size(); }

// Events received from renderer
public:
	void OnRendererSwitch(int newMaxLights);
// End of events received from renderer


// Events sent to renderer
public:
	using OnLightUpdateCallback = std::function<void(const std::vector<Glsl_PointLight>& lights)>;

private:
	OnLightUpdateCallback RC_OnLightUpdate;
// End of events sent to renderer

private:
	entt::registry _registry;

	std::shared_ptr<DirectionalLight> _directionalLight = nullptr;

	std::vector<Glsl_PointLight> _pointLights = std::vector<Glsl_PointLight>();

	uint32_t _rendererMaximumLightCount = 200;

	std::shared_ptr<ImBridge::Bridge> _bridge;
};

}