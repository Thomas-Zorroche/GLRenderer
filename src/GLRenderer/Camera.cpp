#include "Camera.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"

namespace glrenderer {

	Camera::Camera()
	{
		updateVectors();

		updateViewMatrix();
		updateProjectionMatrix();
	}

	Camera::~Camera()
	{

	}

	void Camera::zoom()
	{
		_fov += 2.0;
		updateProjectionMatrix();
	}

	void Camera::updateAspectRatio(float aspectRatio)
	{
		_aspectRatio = aspectRatio;
		updateProjectionMatrix();
	}

	void Camera::updateViewMatrix()
	{
		_viewMatrix = lookAt(_position, _position + _frontVector, _upVector);
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
			glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch)),
			glm::sin(glm::radians(m_pitch)),
			glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch))
		};

		_frontVector = glm::normalize(_frontVector);
		_rightVector = glm::normalize(glm::cross(_frontVector, glm::vec3(0.0f, 1.0f, 0.0f)));
		_upVector = glm::normalize(glm::cross(_rightVector, _frontVector));
	}
}