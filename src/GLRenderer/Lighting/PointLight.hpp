#pragma once

#include "BaseLight.hpp"
#include <unordered_map>

namespace glrenderer {

	struct Glsl_PointLight
	{
		glm::vec3 position;
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
		PointLight(const glm::vec3& color = glm::vec3(1.0, 1.0, 1.0), float intensity = 1.0f);

		~PointLight() = default;

		PointLight * isPointLight() override { return this; }
		DirectionalLight* isDirectionalLight() override { return nullptr; }

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