#include "BaseLight.hpp"

#include "PointLight.hpp"
#include "DirectionalLight.hpp"

namespace glrenderer {



	std::shared_ptr<BaseLight> BaseLight::createLight(LightType type)
	{
		switch (type)
		{
			case glrenderer::LightType::Point:		   return std::make_shared<PointLight>();
			case glrenderer::LightType::Directional:   return std::make_shared<DirectionalLight>();
			case glrenderer::LightType::Spot:          return nullptr;
			case glrenderer::LightType::Area:          return nullptr;
			default:						           return std::make_shared<PointLight>();
		}
	}

}