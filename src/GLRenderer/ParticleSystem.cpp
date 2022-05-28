#include "ParticleSystem.hpp"

#include "Scene/Component.hpp"

#include "Shader.h"

namespace glrenderer
{

int ParticleSystem::instance = 0;


ParticleSystem::ParticleSystem(Scene* scene, uint32_t firstLightIndex)
{

	std::random_device randomDevice;
	_generator = std::mt19937(randomDevice());

	_distributionX = std::uniform_real_distribution<float>(-1.0f, 1.0f);
	_distributionY = std::uniform_real_distribution<float>(-1.0f, 1.0f);
	_distributionZ = std::uniform_real_distribution<float>(-1.0f, 1.0f);

	instance++;

	_name += std::to_string(instance);

	_scene = scene;

	_lights.reserve(_maxParticuleCount);
	OnCountChanged(_startCount);
	_lights[0]->SetOffsetIndex(firstLightIndex);

	OnEmitterTypeChanged(1);

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
		1,
		_maxParticuleCount,
		"Number of particle instances",
		[this](int count) {this->OnCountChanged(count); });
	_bridge->addInt(
		"Seed",
		_seed,
		0,
		_maxSeed,
		"",
		[this](int seed) {this->OnSeedChanged(seed); });
	_bridge->addBool(
		"Show Emitter",
		_showEmitter,
		"Show shape emitter in wireframe");

	_initialize = true;
}

void ParticleSystem::SetLightOffsetIndex(uint32_t offsetIndex)
{
	if (_lights.size() > 0)
	{
		_lights[0]->SetOffsetIndex(offsetIndex);
	}
}

void ParticleSystem::OnEmitterTypeChanged(unsigned int emitterTypeId)
{
	EEmitterType emitterTypeIn = static_cast<EEmitterType>(emitterTypeId);

	if (emitterTypeIn == _emitterType)
	{
		return;
	}

	_emitterType = emitterTypeIn;

	switch (_emitterType)
	{
	case glrenderer::ParticleSystem::EEmitterType::Plan:
	{
		_emitter = _scene->CreateBaseEntity(EBaseEntityType::Plan);
		break;
	}
	case glrenderer::ParticleSystem::EEmitterType::Box:
	{
		_emitter = _scene->CreateBaseEntity(EBaseEntityType::Cube);
		break;
	}
	}

	// Set callback
	auto& callbackComp = _emitter.addComponent<CallbackComponent>();
	callbackComp.OnTransformCallback = std::bind(&ParticleSystem::OnEmitterTransformChanged, this);

	// Set Wireframe material
	auto& mesh = _emitter.getComponent<MeshComponent>().mesh;
	auto& material = mesh->getMaterial();
	material->setShader(_wireframeShader);
}

void ParticleSystem::OnParticleTypeChanged(unsigned int particleTypeId)
{
	_particleType = static_cast<EParticleType>(particleTypeId);

}

void ParticleSystem::OnCountChanged(int count)
{
	// Remove lights
	if (_count > count && count > 0 && count < _maxParticuleCount)
	{
		for (size_t i = 0; i < _count - count; i++)
		{
			_lights.pop_back();
		}
		_scene->RemoveLights(_count - count);
		_count = count;
	}

	// Add lights
	else if (_count < count && count > 0 && count < _maxParticuleCount)
	{
		size_t offset = _lights.size();
		for (size_t i = 0; i < count - _count; i++)
		{
			auto light = std::make_shared<PointLight>();

			// Color
			glm::vec3& color = light->getColor();
			color.r = _distributionColor(_generator);
			light->UpdateDiffuse();

			// Position
			glm::vec3 position;
			position.x = _distributionX(_generator);
			position.y = _distributionY(_generator);
			position.z = _distributionZ(_generator);
			light->UpdateLocation(position);

			_lights.push_back(light);
		}
		auto itStart = _lights.begin() + offset;
		_scene->AddInstancedLights(count - _count, itStart, !_initialize);
		_count = count;
	}
}

void ParticleSystem::OnSeedChanged(int seed)
{

}

void ParticleSystem::OnEmitterTransformChanged()
{
	const glm::vec3& location = _emitter.getComponent<TransformComponent>().location;
	const glm::vec3& scale = _emitter.getComponent<TransformComponent>().scale;

	_distributionX = std::uniform_real_distribution<float>(location.x - scale.x, location.x + scale.x);
	_distributionY = std::uniform_real_distribution<float>(location.y - scale.y, location.y + scale.y);
	_distributionZ = std::uniform_real_distribution<float>(location.z - scale.z, location.z + scale.z);

	UpdateParticuleSystem();
}

uint32_t ParticleSystem::GetFirstLightOffsetIndex() const
{
	if (_lights.size() > 0)
	{
		return _lights[0]->GetOffsetIndex();
	}
}

uint32_t ParticleSystem::GetCount() const
{
	return _count;
}

void ParticleSystem::UpdateParticuleSystem()
{
	for (size_t i = 0; i < _lights.size(); i++)
	{
		auto& light = _lights[i];

		// Color
		glm::vec3& color = light->getColor();
		color.r = _distributionColor(_generator);
		light->UpdateDiffuse();

		// Position
		glm::vec3 position;
		position.x = _distributionX(_generator);
		position.y = _distributionY(_generator);
		position.z = _distributionZ(_generator);
		light->UpdateLocation(position);
	}
	_scene->UpdateLights(_lights);
}







}