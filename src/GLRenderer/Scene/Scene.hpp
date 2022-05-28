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
// Particule System
	const std::vector<std::shared_ptr< class ParticleSystem>>& GetParticuleSystems();
	std::shared_ptr< class ParticleSystem>& GetParticuleSystemAtIndex(uint32_t index);

	void AddParticuleSystem();
	void RemoveParticuleSystemAtIndex(uint32_t index);
// end of particule system


// Lighting
public:
	void AddLights(uint32_t count, std::vector<std::shared_ptr<PointLight>> lights);
	void RemoveLights(uint32_t count);
	void UpdateLights(const std::vector<std::shared_ptr<PointLight>>& lights);
	// On point light or particule system with point lights selected
	void OnLightBufferSelected(uint32_t index, uint32_t count = 1);

	size_t getPointLightNum() const { return _pointLights.size(); }

private:
	std::vector<PointLightData> _pointLights = {};
	std::vector<uint32_t> _lightOffsets = {};
// end of lighting


public:
// Entity Management
	bool ImportModel(const std::string& modelPath, const uint32_t& meshGroupId);

	Entity CreateBaseEntity(EBaseEntityType baseEntityType);

	Entity Duplicate(Entity entityToDuplicate);
	
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
	//void createLights(size_t numLights);
	//void updatePointLights(size_t pointLightsNum);

public:
	//void UpdateLight(std::shared_ptr<PointLight>& light);

// Events received from renderer
public:
	void OnRendererSwitch(int newMaxLights);
// End of events received from renderer


// Events sent to renderer
public:
	using OnLightUpdateCallback = std::function<void(uint32_t lightCount, int startBytesOffset, 
		uint32_t modifiedLightCount, PointLightData* data)>;

private:
	OnLightUpdateCallback RC_OnLightUpdate;
// End of events sent to renderer

private:
	std::shared_ptr<ImBridge::Bridge> _bridge;

private:
	entt::registry _registry;

	std::shared_ptr<DirectionalLight> _directionalLight = nullptr;

	std::vector<std::shared_ptr< class ParticleSystem>> _particleSystems = {};

private:
	uint32_t _rendererMaximumLightCount = 200;
};

}