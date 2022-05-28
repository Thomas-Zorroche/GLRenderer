#pragma once

#include <cstdint>

#include "Scene/Entity.hpp"

#include "ImBridge/Bridge.hpp"

namespace glrenderer
{



class ParticleSystem
{
public:
	ParticleSystem();

	const std::string& GetName() const { return _name; }

	const std::shared_ptr<ImBridge::Bridge>& GetBridge() { return _bridge; }

private:
	void OnEmitterTypeChanged(unsigned int emitterTypeId);
	void OnParticleTypeChanged(unsigned int particleTypeId);
	void OnCountChanged(int count);
	void OnSeedChanged(int seed);

private:
// Particle System data
	enum class EEmitterType
	{
		Plan = 0,
		Box
	};

	EEmitterType _emetterType = EEmitterType::Plan;

	enum class EParticleType
	{
		PointLight = 0,
		Entity,
		Model
	};

	EParticleType _particleType = EParticleType::PointLight;

	int _count = 10;

	int _seed = 0;

	bool _showEmitter = false;

// End of particle system data

	std::string _name = "Particle System";

private:
	static const uint32_t _maxParticuleCount = 1000;
	static const uint32_t _maxSeed = 10000;

private:
	std::shared_ptr<ImBridge::Bridge> _bridge = nullptr;
};

}