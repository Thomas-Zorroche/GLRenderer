#pragma once

#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "Shader.h"

namespace glrenderer {

	class Material
	{
	public:
		Material(const std::shared_ptr<Shader>& shader, const std::string& name = "Material");

		void bind();
		void unbind();

		void setDiffuse(const glm::vec3& value);
		glm::vec3& diffuse() { return _diffuse; }
		void updateDiffuse() { setDiffuse(_diffuse); }


		void setProjectionMatrix(const glm::mat4& projection);
		void setModelMatrix(const glm::mat4& model);

	private:
		std::shared_ptr<Shader> _shader;
		std::string _name;

		glm::vec3 _diffuse = glm::vec3(0.9f);
	};
}