#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace glrenderer {

	class PointLight;
	class DirectionalLight;

	enum class LightType
	{
		Point = 0, Directional, Spot, Area
	};

	class BaseLight
	{
	public:
		BaseLight(LightType type = LightType::Point, const glm::vec3& color = glm::vec3(1.0, 1.0, 1.0), float intensity = 1.0f)
			: _color(color), _intensity(intensity) {}

		virtual ~BaseLight() = default;

		virtual PointLight * isPointLight() = 0;
		virtual DirectionalLight* isDirectionalLight() = 0;

		virtual std::shared_ptr<BaseLight> Clone() = 0;

		static std::shared_ptr<BaseLight> createLight(LightType type);

		const glm::vec3& getColor() const { return _color; }
		glm::vec3& getColor() { return _color; }

		const float& getIntensity() const { return _intensity; }
		float& getIntensity() { return _intensity; }

	protected:
		glm::vec3 _color;
		float _intensity;
	};
}