#pragma once

#include <cstdint>

#include "Scene/Entity.hpp"

#include "ImBridge/Bridge.hpp"

#include <random>

namespace glrenderer
{



class ParticleSystem
{
public:
	ParticleSystem(Scene* scene, uint32_t firstLightIndex, uint32_t sceneIndex);

	// TEMP
	static int instance;

	const std::string& GetName() const { return _name; }

	const std::shared_ptr<ImBridge::Bridge>& GetBridge() { return _bridge; }

	uint32_t GetFirstLightOffsetIndex() const;
	uint32_t GetCount() const;

	Entity GetEmitter() { return _emitter; }

private:
	void UpdateParticuleSystem();

	void OnEmitterTypeChanged(unsigned int emitterTypeId);
	void OnParticleTypeChanged(unsigned int particleTypeId);
	void OnCountChanged(int count);
	void OnSeedChanged(int seed);
	void OnEmitterTransformChanged();
	void OnShowEmitterChanged(bool showEmitter);

private:
// Particle System data
	enum class EEmitterType
	{
		Plan = 0,
		Box
	};

	EEmitterType _emitterType = EEmitterType::Plan;

	enum class EParticleType
	{
		PointLight = 0,
		Entity,
		Model
	};

	EParticleType _particleType = EParticleType::PointLight;

	int _count = 0;
	int _lastCount = 0;

	int _seed = 0;

	bool _showEmitter = true;

// End of particle system data

	std::string _name = "Particle System";

private:
	static const uint32_t _maxParticuleCount = 1000;
	static const uint32_t _maxSeed = 10000;
	static const uint32_t _startCount = 10;

private:
	std::mt19937 _generator;
	std::uniform_real_distribution<float> _distributionColor;

	std::uniform_real_distribution<float> _distributionX;
	std::uniform_real_distribution<float> _distributionY;
	std::uniform_real_distribution<float> _distributionZ;

private:
	std::shared_ptr<ImBridge::Bridge> _bridge = nullptr;

	Scene* _scene = nullptr;

	bool _initialize = false;

	Entity _emitter;

	std::shared_ptr<Shader> _wireframeShader = std::make_shared<Shader>("res/shaders/FlatColor.vert", "res/shaders/Wireframe.frag");

	uint32_t _sceneIndex = 0;

// Light data
public:
	void SetLightOffsetIndex(uint32_t offsetIndex);

private:
	std::vector<std::shared_ptr<PointLight>> _lights = {};
// end of light data
};

}