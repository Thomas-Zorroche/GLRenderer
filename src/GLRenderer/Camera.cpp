#include "Camera.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"

#include <iostream>

namespace glrenderer {

	Camera::Camera()
	{
		updateVectors();
		updateProjectionMatrix();
	}

	Camera::~Camera()
	{

	}

	void Camera::zoom(float deltaY)
	{
		_distance -= deltaY * 0.1f;
		if (_distance < 1.0f)
		{
			_distance = 1.0f;
		}
	}

	void Camera::pan(const glm::vec2& delta)
	{
		_targetPoint += -_rightVector * delta.x * _distance * 0.01f;
		_targetPoint += _upVector * delta.y * _distance * 0.01f;
	}

	void Camera::rotate(const glm::vec2& delta)
	{
		float yawSign = (_upVector.y > 0) ? 1.0f : -1.0f;

		_yaw += yawSign * delta.x * 0.5f;
		_pitch -= delta.y * 0.5f;
		// TODO: remove this
		if (glm::abs(_pitch) > 89.0f)
		{
			_pitch += delta.y * 0.5f;
		}
	}

	void Camera::updateAspectRatio(float aspectRatio)
	{
		_aspectRatio = aspectRatio;
		updateProjectionMatrix();
	}

	void Camera::updateViewMatrix()
	{
		_viewMatrix = lookAt(_position, _targetPoint, _upVector);
		_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
	}

	void Camera::updateProjectionMatrix()
	{
		_projectionMatrix = glm::perspective(glm::radians(_fov), _aspectRatio, _nearClip, _farClip);
		_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
	}

	void Camera::updateVectors()
	{
		_frontVector = {
			glm::cos(glm::radians(_yaw)) * glm::cos(glm::radians(_pitch)),
			glm::sin(glm::radians(_pitch)),
			glm::sin(glm::radians(_yaw)) * glm::cos(glm::radians(_pitch))
		};

		_frontVector = glm::normalize(_frontVector);
		_rightVector = glm::normalize(glm::cross(_frontVector, glm::vec3(0.0f, 1.0f, 0.0f)));
		_upVector = glm::normalize(glm::cross(_rightVector, _frontVector));

		// Update Position
		_position = _targetPoint - _frontVector * _distance;

		updateViewMatrix();
	}
}