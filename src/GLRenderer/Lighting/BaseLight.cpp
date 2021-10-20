#include "BaseLight.hpp"

#include "PointLight.hpp"

namespace glrenderer {



	std::shared_ptr<BaseLight> BaseLight::createLight(LightType type)
	{
		switch (type)
		{
			case glrenderer::LightType::Point: return std::make_shared<PointLight>();
			case glrenderer::LightType::Dir:   return nullptr;
			case glrenderer::LightType::Spot:  return nullptr;
			case glrenderer::LightType::Area:  return nullptr;
			default:						   return std::make_shared<PointLight>();
		}
	}

}