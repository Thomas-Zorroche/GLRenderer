#include "Material.hpp"
#include "Math/Math.hpp"

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

	void Material::bind()
	{
		_shader->Bind();
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


}