#include "ParticleSystem.hpp"

namespace glrenderer
{

ParticleSystem::ParticleSystem()
{
	_bridge = std::make_shared<ImBridge::Bridge>();

	_bridge->addCombo(
		"Emitter Type",
		"Plan\0Box\0\0",
		2,
		[this](unsigned int id) {this->OnEmitterTypeChanged(id); },
		"Type of shape where the particle instances will be emitted");
	_bridge->addCombo(
		"Particle Type",
		"PointLight\0\0",
		1,
		[this](unsigned int id) {this->OnParticleTypeChanged(id); },
		"Type of particle instance");
	_bridge->addInt(
		"Count",
		_count,
		0,
		_maxParticuleCount,
		"Number of particle instances",
		[this](int count) {this->OnCountChanged(count); });
	_bridge->addInt(
		"Seed",
		_seed,
		0,
		_maxSeed,
		"",
		[this](int count) {this->OnCountChanged(count); });
	_bridge->addBool(
		"Show Emitter",
		_showEmitter,
		"Show shape emitter in wireframe");
}

void ParticleSystem::OnEmitterTypeChanged(unsigned int emitterTypeId)
{
	_emetterType = static_cast<EEmitterType>(emitterTypeId);
}

void ParticleSystem::OnParticleTypeChanged(unsigned int particleTypeId)
{
	_particleType = static_cast<EParticleType>(particleTypeId);

}

void ParticleSystem::OnCountChanged(int count)
{

}

void ParticleSystem::OnSeedChanged(int seed)
{

}





}