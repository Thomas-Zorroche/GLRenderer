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

		float getRadius() const { return _radius; }
		void setRadius(float radius);

		float getLinear() const { return _linear; }
		float getQuadratic() const { return _quadratic; }

	private:
		float _radius = 20.0f;

		// Attenuation values
		const float _constant = 20.0f;
		float _linear;
		float _quadratic;

		static const std::vector<glm::vec3> s_attenuationTable;
	};
}