#include "Entity.hpp"

#include "Component.hpp"

namespace glrenderer {

	Entity::Entity(entt::entity id, Scene* scene, const std::string& label)
		: _entityId(id), _scene(scene), _label(label)
	{

	}

	Entity::~Entity()
	{

	}
}