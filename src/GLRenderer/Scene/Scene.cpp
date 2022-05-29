#include "Scene.hpp"

#include "Component.hpp"
#include "../Mesh.hpp"
#include "Entity.hpp"

#include "ImBridge/Bridge.hpp"
#include "ImBridge/Parameter.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "../Properties/Render/ShadowsProperties.hpp"

#include "../Renderer/IRenderer.hpp"
#include "../Renderer/RendererContext.hpp"

#include "../ParticleSystem.hpp"

// TEMP
#include "GLRenderer/Lighting/DirectionalLight.hpp"

#include "../Framebuffer.hpp"
#include "../Camera.hpp"

#include "../Loader/GlTFLoader.hpp"

#include <random>
#include <algorithm>


namespace glrenderer {

//glm::mat4 Scene::getLightSpaceMatrix()
//{
//	// TODO member dirLight
//	glm::vec3 lightDirection;
//	glm::mat4 rot;
//	float offset = 0.0f;

//	float far_plane = 0.0f;
//	float near_plane = 0.0f;

//	auto view = _registry.view<LightComponent>();
//	for (auto& entityId : view)
//	{
//		Entity entity = { entityId, this };
//		auto light = entity.getComponent<LightComponent>().light;
//		DirectionalLight* dirLight = light->isDirectionalLight();
//		if (dirLight)
//		{
//			far_plane = dirLight->getFarPlane();
//			near_plane = dirLight->getNearPlane();
//			offset = dirLight->getOffsetPosition();
//			lightDirection = entity.getComponent<LineComponent>().line->getDirection();

//			auto rotation = entity.getComponent<TransformComponent>().rotation;
//			// TODO optimize this
//			rot = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));
//			rot = glm::rotate(rot, glm::radians(rotation.y), glm::vec3(0, 1, 0));
//			rot = glm::rotate(rot, glm::radians(rotation.x), glm::vec3(1, 0, 0));

//			break;
//		}
//	}

//	glm::mat4 lightView = glm::lookAt(
//		glm::vec3(rot * glm::vec4(lightDirection, 1.0)) * glm::vec3(offset), // position
//		glm::vec3(rot * glm::vec4(lightDirection, 1.0)),   // target
//		glm::vec3(0.0f, 1.0f, 0.0f)    // up vector
//	);
//	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

//	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
//	
//	return lightSpaceMatrix;
//}

Scene::Scene(const std::shared_ptr<RendererContext>& rendererContext)
{
	auto OnLightUpdateCallback = std::bind(&RendererContext::OnLightUpdate, rendererContext.get(), 
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	RC_OnLightUpdate = OnLightUpdateCallback;

	_pointLights.reserve(rendererContext->GetGlobalMaxLightCount());

	//static int pointLightsNum = 10;
	//createLights(pointLightsNum);
	_bridge = std::make_shared<ImBridge::Bridge>();
	//_bridge->addInt(
	//	"PointLightNum",
	//	pointLightsNum,
	//	0,
	//	rendererContext->GetRenderer()->GetMaximumLightCount(),
	//	"Number of point lights in the scene",
	//	[this](int value) {this->updatePointLights(value); }
	//);
}

Scene::~Scene()
{

}

//void Scene::createLights(size_t numLights)
//{
//	if (_pointLights.size() + numLights > _rendererMaximumLightCount)
//	{
//		std::cout << "Cannot exceed " << _rendererMaximumLightCount << " point lights.\n";
//		return;
//	}
//		
//	size_t oldSize = _pointLights.size();
//	size_t newSize = oldSize + numLights;
//
//	// Lights
//	std::random_device randomDevice;
//	std::mt19937 generator(randomDevice());
//	std::uniform_real_distribution<float>distributionX(-100.0f, 100.0f);
//	std::uniform_real_distribution<float> distributionZ(-40.0f, 40.0f);
//	std::uniform_real_distribution<float> distributionColor(0.0f, 1.0f);
//
//	int startBytesOffset = sizeof(PointLightData) * _pointLights.size();
//	int indexStart = _pointLights.size();
//	_pointLights.resize(newSize);
//	for (size_t i = oldSize; i < newSize; i++)
//	{
//		auto pointLight = std::make_shared<PointLight>();
//
//		// random color
//		glm::vec3& color = pointLight->getColor();
//		color.r = distributionColor(generator);
//		color.g = distributionColor(generator);
//		color.b = distributionColor(generator);
//
//		_pointLights[i] = &pointLight->GetLightData();
//		_pointLights[i]->location = { distributionX(generator), 0.7f, distributionZ(generator) };
//	}
//
//	RC_OnLightUpdate(_pointLights.size(), startBytesOffset, numLights, _pointLights[indexStart]);
//}

//void Scene::updatePointLights(size_t pointLightsNum)
//{
//	int pointLightsNumDelta = _pointLights.size() - pointLightsNum;
//		
//	// Remove lights
//	if (pointLightsNumDelta > 0)
//	{
//		for (size_t i = 0; i < pointLightsNumDelta; i++)
//		{
//			_pointLights.pop_back();
//		}
//		RC_OnLightUpdate(_pointLights.size(), 0, 0, 0);
//	}
//	// Add lights
//	else if (pointLightsNumDelta < 0)
//	{
//		createLights(std::abs(pointLightsNumDelta));
//	}
//}
//
//void Scene::UpdateLight(std::shared_ptr<PointLight>& light)
//{
//	auto it = std::find(_pointLights.begin(), _pointLights.end(), &light->GetLightData());
//	if (it == _pointLights.end())
//	{
//		std::cout << "UpdateLight error" << std::endl;
//		return;
//	}
//	uint32_t lightIndex = it - _pointLights.begin();;
//
//	RC_OnLightUpdate(_pointLights.size(), sizeof(PointLightData) * lightIndex, 1, _pointLights[lightIndex]);
//}

bool Scene::ImportModel(const std::string& modelPath, const uint32_t& meshGroupId)
{
	// ATM, import only gltf
	tinygltf::Model model;
	if (!glTFLoader::loadFile(modelPath, model))
	{
		std::cout << "Cannot open " << modelPath << std::endl;
		return false;
	}

	// create meshes
	std::vector<std::shared_ptr<Mesh>> meshes;
	glTFLoader::createMeshes(model, meshes);

	// create entities
	for (const auto& mesh : meshes)
	{
		auto& meshEntity = CreateEntity("Mesh");
		meshEntity.addComponent<glrenderer::MeshComponent>(mesh);
		auto& transform = meshEntity.getComponent<glrenderer::TransformComponent>();
		static float import_scale = 0.08f;
		transform.scale = glm::vec3(import_scale, import_scale, import_scale);

		// Add into a group
		meshEntity.getComponent<glrenderer::LabelComponent>().groupId = meshGroupId;
	}

	std::cout << "Open " << modelPath << std::endl;
	return true;
}

Entity Scene::CreateBaseEntity(EBaseEntityType baseEntityType)
{
	switch (baseEntityType)
	{
	case glrenderer::EBaseEntityType::Cube:
	{
		Entity& cube = CreateEntity("Cube");
		cube.addComponent<MeshComponent>(glrenderer::Mesh::createMesh(glrenderer::MeshShape::Cube));
		return cube;
		break;
	}
	case glrenderer::EBaseEntityType::Plan:
	{
		Entity& plan = CreateEntity("Plan");
		plan.addComponent<MeshComponent>(glrenderer::Mesh::createMesh(glrenderer::MeshShape::Plan));
		return plan;
		break;
	}
	case glrenderer::EBaseEntityType::Sphere:
		break;
	case glrenderer::EBaseEntityType::PointLight:
	{
		Entity& pointLightEntity = CreateEntity("Point Light");
		auto& lightComp = pointLightEntity.addComponent<LightComponent>(BaseLight::createLight(LightType::Point));

		if (auto pointLight = std::dynamic_pointer_cast<PointLight>(lightComp.light))
		{
			AddLight(pointLight);
		}
		return pointLightEntity;
		break;
	}
	case glrenderer::EBaseEntityType::DirectionalLight:
	{
		Entity& directionalLight = CreateEntity("Directional Light");
		directionalLight.addComponent<LightComponent>(BaseLight::createLight(LightType::Directional));
		auto& transformDirLight = directionalLight.getComponent<TransformComponent>();
		transformDirLight.rotation = { 1.0, 1.0, 1.0 };
		return directionalLight;
		break;
	}
	case glrenderer::EBaseEntityType::SpotLight:
		break;
	case glrenderer::EBaseEntityType::ParticuleSystem:
		break;
	}
}

Entity Scene::CreateEntity(const std::string& name)
{
	Entity entity = { _registry.create(), this };
	auto& labelEntity = entity.addComponent<LabelComponent>();
	labelEntity.label = (name == "") ? "Entity" : name;

	// Avoid duplicate labels
	// TODO: Optimize this
	if (isLabelDuplicate(labelEntity.label))
		makeUniqueLabel(labelEntity.label);

	entity.addComponent<TransformComponent>();
	return entity;
}

Entity Scene::Duplicate(Entity baseEntity)
{
	auto& labelEntity = baseEntity.getComponent<LabelComponent>();
	Entity entityDuplicated = CreateEntity(labelEntity.label);

	// If exists, copy mesh
	if (baseEntity.hasComponent<MeshComponent>())
	{
		auto mesh = baseEntity.getComponent<MeshComponent>().mesh;
		entityDuplicated.addComponent<MeshComponent>(mesh);
	}

	// If exists, copy light
	if (baseEntity.hasComponent<LightComponent>())
	{
		auto& baseLight = baseEntity.getComponent<LightComponent>().light;
		auto& light = entityDuplicated.addComponent<LightComponent>(baseLight->Clone()).light;
		if (auto& pointLight = std::dynamic_pointer_cast<PointLight>(light))
			AddLight(pointLight);
	}

	// Always copy transform
	auto& transformBase = baseEntity.getComponent<TransformComponent>();
	auto& transformDuplicated = entityDuplicated.getComponent<TransformComponent>();

	transformDuplicated.location = transformBase.location;
	transformDuplicated.rotation = transformBase.rotation;
	transformDuplicated.scale = transformBase.scale;

	return entityDuplicated;
}

void Scene::RenameEntity(Entity& entity, const std::string& name)
{
	std::string& labelEntitySelected = entity.getComponent<glrenderer::LabelComponent>().label;
	labelEntitySelected = name;
	makeUniqueLabel(labelEntitySelected);
}


void Scene::makeUniqueLabel(std::string& label)
{
	if (!isLabelDuplicate(label))
		return;

	uint32_t index = 1;
	std::string baseLabel = label;
	do
	{
		label = baseLabel + std::to_string(index);
		index++;
	} while (isLabelDuplicate(label));
}

bool Scene::isLabelDuplicate(const std::string& label)
{
	auto view = _registry.view<LabelComponent>();
	int duplicate = 0;
	for (auto& entityId : view)
	{
		Entity entity = { entityId, this };
		std::string& labelOther = entity.getComponent<LabelComponent>().label;

		if (label == labelOther)
		{
			duplicate++;
			if (duplicate >= 2)
				return true;
		}
	}
	return false;
}

void Scene::OnRendererSwitch(int newMaxLights)
{
	_rendererMaximumLightCount = newMaxLights;
}



// -----------------------------------------------------------------------------------
// Particule System 
// -----------------------------------------------------------------------------------

void Scene::AddParticuleSystem()
{
	// Will be changed at particle system initialisation
	auto firstLight = std::make_shared<PointLight>();
	_pointLights.push_back(firstLight->GetLightData());
	_lightOffsets.push_back(_pointLights.size() - 1);

	_particleSystems.push_back(std::make_shared<ParticleSystem>(this, _lightOffsets.size() - 1, _particleSystems.size()));
}

const std::vector<std::shared_ptr< class ParticleSystem>>& Scene::GetParticuleSystems()
{
	return _particleSystems;
}

std::shared_ptr<ParticleSystem>& Scene::GetParticuleSystemAtIndex(uint32_t index)
{
	if (index < _particleSystems.size())
	{
		return _particleSystems[index];
	}
}

void Scene::RemoveParticuleSystemAtIndex(uint32_t index)
{
	if (index < _particleSystems.size())
	{
		_particleSystems.erase(_particleSystems.begin() + index);
	}
}

void Scene::OnParticleSystemSelected(uint32_t PSIndex)
{
	if (PSIndex >= _particleSystems.size())
	{
		std::cout << "OnParticleSystemSelected error : out of range" << std::endl;
		return;
	}
	// Retrieve particle system offset
	uint32_t firstLightIndex = _particleSystems[PSIndex]->GetFirstLightOffsetIndex();
	// Already last index in the list, so no need to rotate
	if (firstLightIndex == _lightOffsets.size() - 1)
		return;

	uint32_t offset = _lightOffsets[firstLightIndex];
	uint32_t count = _particleSystems[PSIndex]->GetCount();

	// Initialize some data to update renderer UBO
	size_t startBytesOffset = offset * sizeof(PointLightData);
	PointLightData* dataOut = &_pointLights[offset];

	// Move light to the end of the vector
	auto itOldStart = _pointLights.begin() + offset;
	auto itNewStart = std::rotate(itOldStart, itOldStart + count, _pointLights.end());

	// Decrease lights offsets
	for (uint32_t index = firstLightIndex + 1; index < _lightOffsets.size() - 1; ++index)
	{
		_lightOffsets[index] -= count;
	}
	_lightOffsets[firstLightIndex] = _pointLights.size() - count;

	// Update Renderer UBO
	RC_OnLightUpdate(_pointLights.size(), startBytesOffset, 1, dataOut);
}

// -----------------------------------------------------------------------------------
// Lighting 
// -----------------------------------------------------------------------------------

void Scene::AddLight(const std::shared_ptr<PointLight>& light)
{
	if (_pointLights.size() + 1 > _rendererMaximumLightCount)
	{
		std::cout << "Cannot exceed " << _rendererMaximumLightCount << " point lights.\n";
		return;
	}

	size_t sizeAtStart = _pointLights.size();
	_pointLights.push_back(light->GetLightData());

	// Add index in offset array
	_lightOffsets.push_back(_pointLights.size() - 1);
	light->SetOffsetIndex(_lightOffsets.size() - 1);

	size_t startBytesOffset = sizeAtStart * sizeof(PointLightData);
	PointLightData* dataOut = &_pointLights[sizeAtStart];

	// Update Renderer UBO
	RC_OnLightUpdate(_pointLights.size(), startBytesOffset, 1, dataOut);
}

void Scene::AddInstancedLights(uint32_t count, std::vector<std::shared_ptr<PointLight>>::const_iterator itStart, bool firstAdd)
{
	if (_pointLights.size() + count > _rendererMaximumLightCount)
	{
		std::cout << "Cannot exceed " << _rendererMaximumLightCount << " point lights.\n";
		return;
	}

	// First add, remove first light
	if (firstAdd)
		_pointLights.pop_back();

	size_t sizeAtStart = _pointLights.size();
	for (size_t i = 0; i < count; i++)
	{
		_pointLights.push_back((*itStart)->GetLightData());
		itStart++;
	}

	size_t startBytesOffset = sizeAtStart * sizeof(PointLightData);
	PointLightData* dataOut = &_pointLights[sizeAtStart];

	// Update Renderer UBO
	RC_OnLightUpdate(_pointLights.size(), startBytesOffset, count, dataOut);
}

void Scene::RemoveLights(uint32_t count)
{
	for (size_t i = 0; i < count; i++)
	{
		if (!_pointLights.empty())
			_pointLights.pop_back();
	}

	// Update Renderer UBO
	RC_OnLightUpdate(_pointLights.size(), 0, 0, 0);
}

void Scene::UpdateLights(const std::vector<std::shared_ptr<PointLight>>& lights)
{
	uint32_t startIndex = lights[0]->GetOffsetIndex();
	size_t startBytesOffset = startIndex * sizeof(PointLightData);
	PointLightData* dataOut = &_pointLights[startIndex];

	PointLightData* it = dataOut;
	for (const auto& light : lights)
	{
		light->CopyLightData(it);
		it++;
	}

	// Update Renderer UBO
	RC_OnLightUpdate(_pointLights.size(), startBytesOffset, lights.size(), dataOut);
}


//void Scene::sendLightingUniforms(const std::shared_ptr<Shader>& shader)
//{
//	auto viewLight = _registry.view<LightComponent>();

//	// TEMP
//	// Only one directional light can be used
//	bool dirLightUsed = false;

//	uint32_t lightIndex = 0;
//	for (auto entityLight : viewLight)
//	{
//		//if (lightIndex > 7)
//		//	return;

//		std::string lightIndexStr = std::to_string(lightIndex);

//		Entity entity = { entityLight, this };
//		std::shared_ptr<BaseLight>& baseLight = entity.getComponent<LightComponent>().light;

//		PointLight* pointLight = baseLight->isPointLight();
//		if (pointLight)
//		{
//			//if (lightIndex > 7)
//			//	continue;

//			shader->SetUniform3f("pointLights[" + lightIndexStr + "].diffuse", baseLight->getColor());
//			shader->SetUniform3f("pointLights[" + lightIndexStr + "].ambient", baseLight->getColor());
//			shader->SetUniform3f("pointLights[" + lightIndexStr + "].specular", { 1.0, 1.0, 1.0 });
//			shader->SetUniform1f("pointLights[" + lightIndexStr + "].intensity", baseLight->getIntensity());

//			glm::vec3& position = entity.getComponent<TransformComponent>().location;
//			shader->SetUniform3f("pointLights[" + lightIndexStr + "].position", position);
//			shader->SetUniform1f("pointLights[" + lightIndexStr + "].linear", pointLight->getLinear());
//			shader->SetUniform1f("pointLights[" + lightIndexStr + "].quadratic", pointLight->getQuadratic());

//			++lightIndex;
//			continue;
//		}

//		DirectionalLight* dirLight = baseLight->isDirectionalLight();
//		if (dirLight && !dirLightUsed)
//		{
//			dirLightUsed = true;

//			shader->SetUniform3f("directionalLight.diffuse", baseLight->getColor());
//			shader->SetUniform3f("directionalLight.ambient", { 0.5, 0.5, 0.5 });
//			shader->SetUniform3f("directionalLight.specular", { 1.0, 1.0, 1.0 });
//			shader->SetUniform1f("directionalLight.intensity", baseLight->getIntensity());

//			if (Renderer::getShadowProperties()->getSoftShadows())
//			{
//				shader->SetUniform1f("directionalLight.size", _directionalLight->getSize());
//				//float frustumSize = 2 * _directionalLight->getNearPlane() * std::tanf(60.0f * 0.5f) * 1.0f; // TODO REMOVE THIS
//				//shader->SetUniform1f("directionalLight.frustumSize", _directionalLight->getFrustumSize());
//				shader->SetUniform1f("directionalLight.nearPlane", _directionalLight->getNearPlane());
//			}

//			auto& line = entity.getComponent<LineComponent>().line;
//			auto rotation = entity.getComponent<TransformComponent>().rotation;
//			// TODO optimize this
//			glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));
//			rot = glm::rotate(rot, glm::radians(rotation.y), glm::vec3(0, 1, 0));
//			rot = glm::rotate(rot, glm::radians(rotation.x), glm::vec3(1, 0, 0));

//			shader->SetUniform3f("directionalLight.direction", rot * glm::vec4(line->getDirection(), 1.0f));

//			++lightIndex;
//			continue;
//		}

//	}
//}

}