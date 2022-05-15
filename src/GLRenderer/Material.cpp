﻿#include "Material.hpp"
#include "Math/Math.hpp"

#include "glad/glad.h"

// TEMP
#include <iostream>

namespace glrenderer {

	Material::Material(const std::shared_ptr<Shader>& shader, const std::string& name)
		: _shader(shader), _name(name)
	{
		if (_shader)
		{
			updateDiffuse();
			updateRoughness();
		}
	}

	void Material::bind() const
	{
		_shader->Bind();

		if (_baseColorTexture >= 0)
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, _baseColorTexture);
		}
	}

	void Material::unbind()
	{
		_shader->Unbind();

	}

	void Material::updateDiffuse()
	{
		bind();
		_shader->SetUniform3f("uColor", _diffuse);
	}

	void Material::updateRoughness()
	{
		_shininess = math::lerp(0.0f, 900.0f, 1.0 -_roughness);

		bind();
		_shader->SetUniform1f("uShininess", _shininess);
	}

	void Material::setBaseColorTexture(int texture, const glm::vec4& baseColorFactor)
	{
		bind();

		_baseColorTexture = texture;
		_shader->SetUniform1i("uBaseColorTexture", 3);
		_shader->SetUniform4f("uBaseColorFactor", baseColorFactor);
	}

}