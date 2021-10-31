#pragma once

#include "BaseLight.hpp"

namespace glrenderer {

	class DirectionalLight : public BaseLight
	{
	public:
		DirectionalLight();

		~DirectionalLight();

		PointLight* isPointLight() override { return nullptr; }
		DirectionalLight* isDirectionalLight() override { return this; }

		float getFarPlane() const { return _farPlane; }
		float& getFarPlane() { return _farPlane; }

		float getNearPlane() const { return _nearPlane; }
		float& getNearPlane() { return _nearPlane; }

		float getOffsetPosition() const { return _offestPosition; }
		float& getOffsetPosition() { return _offestPosition; }

		float getSize() const { return _size; }
		float& getSize() { return _size; }

		float getFrustumSize() const { return _frustumSize; }
		float& getFrustumSize() { return _frustumSize; }

		bool getSoftShadow() const { return _softShadow; }
		bool& getSoftShadow() { return _softShadow; }


	private:
		float _farPlane = 80.0f;
		float _nearPlane = 4.5f;

		float _offestPosition = -5.8f;

		bool _softShadow = false;

		float _size = 0.015f;
		
		float _frustumSize = 27.9f; // TODO replace by formula
	};
}