#include "Entity.hpp"

#include "Component.hpp"

namespace glrenderer {

	Entity::Entity(entt::entity id, Scene* scene)
		: _entityId(id), _scene(scene)
	{

	}

	Entity::~Entity()
	{

	}

}