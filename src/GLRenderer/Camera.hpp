#pragma once

#include <glm/glm.hpp>

namespace glrenderer {

	class Camera
	{
	public:
		Camera();
		~Camera();

		const glm::mat4& getViewProjectionMatrix() const { return _viewProjectionMatrix; }
		void updateAspectRatio(float aspectRatio);

		// User Actions
		void zoom();

	private:
		void updateViewMatrix();
		void updateProjectionMatrix();

	private:
		float _fov = 45.0f;
		float _aspectRatio = 1.778f;
		float _nearClip = 0.1f;
		float _farClip = 1000.0f;

		glm::vec3 _position = { 0.0f, 0.0f, -5.0f };
		glm::vec3 _upVector;
		glm::vec3 _rightVector;
		// Pointing in the reverse direction
		glm::vec3 _direction;
		glm::vec3 _target = { 0.0f, 0.0f, 0.0f };

		glm::mat4 _viewProjectionMatrix;
		glm::mat4 _viewMatrix;
		glm::mat4 _projectionMatrix;


	};
}