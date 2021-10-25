#pragma once

#include "BaseLight.hpp"
#include <unordered_map>

namespace glrenderer {

	class DirectionalLight : public BaseLight
	{
	public:
		DirectionalLight() {};

		~DirectionalLight() = default;

		PointLight* isPointLight() override { return nullptr; }
		DirectionalLight* isDirectionalLight() override { return this; }

		float getFarPlane() const { return _farPlane; }
		float& getFarPlane() { return _farPlane; }

	private:
		float _farPlane = 20.0f;
	};
}