#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace glrenderer {

	class PointLight;

	enum class LightType
	{
		Point = 0, Dir, Spot, Area
	};

	class BaseLight
	{
	public:
		BaseLight(LightType type = LightType::Point, const glm::vec3& color = glm::vec3(1.0, 1.0, 1.0), float intensity = 1.0f)
			: _color(color), _intensity(intensity) {}

		virtual ~BaseLight() = default;

		virtual PointLight * isPointLight() = 0;

		static std::shared_ptr<BaseLight> createLight(LightType type);

		const glm::vec3& getColor() const { return _color; }
		glm::vec3& getColor() { return _color; }

		const float& getIntensity() const { return _intensity; }
		float& getIntensity() { return _intensity; }

	private:
		glm::vec3 _color;
		float _intensity;
	};
}