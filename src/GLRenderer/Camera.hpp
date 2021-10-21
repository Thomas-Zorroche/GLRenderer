#pragma once

#include <glm/glm.hpp>

namespace glrenderer {

	class Camera
	{
	public:
		Camera();
		~Camera();

		const glm::mat4& getViewProjectionMatrix() const { return _viewProjectionMatrix; }
		const glm::vec3& getPosition() const { return _position; }

		const glm::mat4& getViewMatrix() const { return _viewMatrix; }
		const glm::mat4& getProjectionMatrix() const { return _projectionMatrix; }
		
		void updateAspectRatio(float aspectRatio);

		// User Actions
		void zoom(float deltaY);
		void pan(const glm::vec2& delta);
		void rotate(const glm::vec2& delta);

		void updateVectors();

	private:
		void updateViewMatrix();
		void updateProjectionMatrix();
		

	private:
		float _fov = 65.0f;
		float _aspectRatio = 1.778f;
		float _nearClip = 0.1f;
		float _farClip = 1000.0f;

		// Euler Angles
		float _yaw = 47.0f;
		float _pitch = -28.0f;

		glm::vec3 _position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 _targetPoint = { 0.0f, 0.0f, 0.0f };
		// Distance from target point
		float _distance = 15.0f;

		glm::vec3 _frontVector = { 0.0f, 0.0f, -1.0f };
		glm::vec3 _upVector = { 0.0f, 1.0f, 0.0f };
		glm::vec3 _rightVector;

		glm::mat4 _viewProjectionMatrix;
		glm::mat4 _viewMatrix;
		glm::mat4 _projectionMatrix;


	};
}