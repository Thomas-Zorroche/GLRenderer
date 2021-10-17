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
		
		void updateVectors();

	private:
		float _fov = 65.0f;
		float _aspectRatio = 1.778f;
		float _nearClip = 0.1f;
		float _farClip = 1000.0f;

		// Euler Angles
		float m_yaw = -90.0f;
		float m_pitch = 0.0f;

		glm::vec3 _position = { 0.0f, 0.0f, 4.0f };
		glm::vec3 _frontVector = { 0.0f, 0.0f, -1.0f };
		glm::vec3 _upVector = { 0.0f, 1.0f, 0.0f };
		glm::vec3 _rightVector;

		glm::mat4 _viewProjectionMatrix;
		glm::mat4 _viewMatrix;
		glm::mat4 _projectionMatrix;


	};
}