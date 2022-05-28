#pragma once

#include "BaseLight.hpp"
#include <unordered_map>

namespace glrenderer {

	struct PointLightData
	{
		glm::vec3 location;
		float intensity;

		glm::vec3 ambient;
		float linear;

		glm::vec3 diffuse;
		float quadratic;

		glm::vec4 specular;
	};

	class PointLight : public BaseLight
	{
	public:
		PointLight(const glm::vec3& color = glm::vec3(1.0, 1.0, 1.0), float intensity = 5.0f);

		~PointLight() = default;

		PointLight * isPointLight() override { return this; }
		DirectionalLight* isDirectionalLight() override { return nullptr; }

		float getRadius() const { return _radius; }
		void setRadius(float radius);

		//float getLinear() const { return _linear; }
		//float getQuadratic() const { return _quadratic; }

		const PointLightData& GetLightData() const;
		void CopyLightData(PointLightData* dest);

		void UpdateLocation(const glm::vec3& location);
		void UpdateDiffuse();
		void UpdateIntensity();

		void SetOffsetIndex(uint32_t offset) { _offsetIndex = offset; }
		uint32_t GetOffsetIndex() const { return _offsetIndex; }

		virtual std::shared_ptr<BaseLight> Clone() override;

	private:
		float _radius = 20.0f;

		// Attenuation values
		const float _constant = 20.0f;

		PointLightData _data;

		// Index in scene::_lightOffsets
		int _offsetIndex = -1;

		static const std::vector<glm::vec3> s_attenuationTable;
	};
}