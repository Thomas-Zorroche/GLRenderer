#include "Material.hpp"

namespace glrenderer {

	Material::Material(const std::shared_ptr<Shader>& shader, const std::string& name)
		: _shader(shader), _name(name)
	{
		bind();
		setDiffuse(_diffuse);
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

	void Material::setProjectionMatrix(const glm::mat4& projection)
	{
		_shader->SetUniformMatrix4fv("uProjectionMatrix", projection);
	}

	void Material::setModelMatrix(const glm::mat4& model)
	{
		_shader->SetUniformMatrix4fv("uModelMatrix", model);
	}


}