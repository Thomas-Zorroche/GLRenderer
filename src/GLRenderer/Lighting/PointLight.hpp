#pragma once

#include "BaseLight.hpp"
#include <unordered_map>

namespace glrenderer {

	class PointLight : public BaseLight
	{
	public:
		PointLight(const glm::vec3& color = glm::vec3(1.0, 1.0, 1.0), float intensity = 1.0f);

		~PointLight() = default;

		PointLight * isPointLight() override { return this; }

	private:
		void updateAttenuationValues();

	private:
		float _radius = 1.0f;

		// Attenuation values
		const float _constant = 20.0;
		float _linear;
		float _quadratic;

		static const std::unordered_map<int, std::pair<float, float>> _attenuationTable;
	};
}