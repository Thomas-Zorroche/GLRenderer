#include "Material.hpp"

namespace glrenderer {

	Material::Material(const std::shared_ptr<Shader>& shader, const std::string& name)
		: _shader(shader), _name(name)
	{
		if (_shader)
		{
			bind();
			setDiffuse(_diffuse);
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

	void Material::setDiffuse(const glm::vec3& value)
	{
		_diffuse = value;
		_shader->SetUniform3f("uColor", value);
	}

}