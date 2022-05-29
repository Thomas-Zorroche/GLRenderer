#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <memory>
#include <string>
#include <functional>

#include "../Mesh.hpp"
#include "../Lighting/BaseLight.hpp"
#include "../Line.hpp"

namespace glrenderer {

	struct TransformComponent
	{
		glm::vec3 location = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		glm::mat4 getModelMatrix() const
		{
			glm::mat4 trans = glm::translate(glm::mat4(1.0f), location);

			glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));
			rot = glm::rotate(rot, glm::radians(rotation.y), glm::vec3(0, 1, 0));
			rot = glm::rotate(rot, glm::radians(rotation.x), glm::vec3(1, 0, 0));

			glm::mat4 scl = glm::scale(glm::mat4(1.0f), scale);

			return trans * rot * scl;
		}
	};

	struct LabelComponent
	{
		std::string label = "Entity";
		uint32_t groupId = 0; // default group in world outliner

		LabelComponent() = default;
		LabelComponent(const LabelComponent&) = default;
		LabelComponent(const std::string& label) : label(label) {}
	};

	struct MeshComponent
	{
		std::shared_ptr<Mesh> mesh = nullptr;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const std::shared_ptr<Mesh>& mesh)
			: mesh(mesh) {};
	};

	struct LightComponent
	{
		std::shared_ptr<BaseLight> light = nullptr;

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
		LightComponent(const std::shared_ptr<BaseLight>& light)
			: light(light) {}
	};

	struct LineComponent
	{
		std::shared_ptr<Line> line = nullptr;

		LineComponent() = default;
		LineComponent(const LineComponent&) = default;
		LineComponent(const std::shared_ptr<Line>& line)
			: line(line) {}
	};

	struct ParticleSystemComponent
	{
		// index in scene::particleSystems
		uint32_t index = 0;

		ParticleSystemComponent() = default;
	};

	struct CallbackComponent
	{
		std::function<void()> OnTransformCallback;

		CallbackComponent() = default;
		CallbackComponent(const CallbackComponent&) = default;
		CallbackComponent(std::function<void()> callback)
			: OnTransformCallback(callback) {}
	};


}