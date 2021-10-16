#include "Camera.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"

namespace glrenderer {

	Camera::Camera()
	{
		_direction = glm::normalize(_position - _target);
		_rightVector = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), _direction));
		_upVector = glm::cross(_direction, _rightVector);

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
		_viewMatrix = glm::lookAt(_position, _target, _upVector);
		_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
	}

	void Camera::updateProjectionMatrix()
	{
		_projectionMatrix = glm::perspective(glm::radians(_fov), _aspectRatio, _nearClip, _farClip);
		_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
	}


}