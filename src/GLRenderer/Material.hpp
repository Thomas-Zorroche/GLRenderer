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

		void bind() const;
		void unbind();

		void bindColorTexture() const;

		const glm::vec3& getDiffuse() const { return _diffuse; }
		glm::vec3& getDiffuse() { return _diffuse; }
		void updateDiffuse();
		void setDiffuse(int r, int g, int b);

		const float& getRoughness() const { return _roughness; }
		float& getRoughness() { return _roughness; }
		void updateRoughness();

		const float& getShininess() const { return _shininess; }

		const std::shared_ptr<Shader>& getShader() const { return _shader; }
		void setShader(const std::shared_ptr<Shader>& shader) { _shader = shader; }

		void setBaseColorTexture(int texture, const glm::vec4& baseColorFactor = glm::vec4(1.f, 1.f, 1.f, 1.f));

		bool IsWireframe() const { return _isWireframe; }
		void SetWireframe(bool wireframe) { _isWireframe = wireframe; }

	private:
		std::shared_ptr<Shader> _shader;
		std::string _name;

		glm::vec3 _diffuse = glm::vec3(0.9f);
		float _roughness = 0.5f;
		float _shininess = 450.0f;

		int _baseColorTexture = -1;
		glm::vec4 _baseColorFactor = glm::vec4(1.f, 1.f, 1.f, 1.f);

		bool _isWireframe = false;
	};
}