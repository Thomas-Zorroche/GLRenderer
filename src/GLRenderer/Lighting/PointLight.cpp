#include "PointLight.hpp"

#include <iostream>

#include "../Math/Math.hpp"

namespace glrenderer {

    /*
    * Attenuation values per radius
    * radius  |  Linear  |  Quadratic
    */
    const std::vector<glm::vec3> PointLight::s_attenuationTable = {
        {7.0, 0.7, 1.8 },
        {13.0, 0.35, 0.44 },
        {20.0, 0.22, 0.2 },
        {32.0, 0.14, 0.07 },
        {50.0, 0.09, 0.032 },
        {65.0, 0.07, 0.017 },
        {100.0, 0.045, 0.0075 },
        {160.0, 0.027, 0.0028 },
        {200.0, 0.022, 0.0019 },
        {325.0, 0.014, 0.0007 },
        {600.0, 0.007, 0.0002 }
    };

	PointLight::PointLight(const glm::vec3& color, float intensity)
		: BaseLight(LightType::Point, color, intensity)
	{
        setRadius(_radius);
        
        _data.diffuse = color;
        _data.intensity = intensity;
        _data.ambient = glm::vec3(0, 0, 0);
        _data.specular = glm::vec4(color, 0.0f);
        _data.location = glm::vec3(0, 0, 0);
	}

    const PointLightData& PointLight::GetLightData() const
    {
        return _data;
    }

    void PointLight::UpdateLocation(const glm::vec3& location)
    {
        _data.location = location;
    }

    void PointLight::UpdateDiffuse()
    {
        _data.diffuse = _color;
    }

    void PointLight::UpdateIntensity()
    {
        _data.intensity = _intensity;
    }

    void PointLight::CopyLightData(PointLightData* dest)
    {
        *dest = _data;
    }

    std::shared_ptr<BaseLight> PointLight::Clone()
    {
        return std::make_shared<PointLight>(*this);
    }

    void PointLight::setRadius(float radius)
    {
        if (radius <= 7.0f)
        {
            _data.linear = 0.7;
            _data.quadratic = 1.8;
            return;
        }

        if (radius >= 600.0)
        {
            _data.linear = 0.007;
            _data.quadratic = 0.0002;
            return;
        }

        _radius = radius;
        const float epsilon = 0.1f;

        // Lerp for retrieve linear and quadratic
        glm::vec3 minRadius = { 7.0, 0.7, 1.8 };
        glm::vec3 maxRadius = { 600, 0.007, 0.0002 };
        for (const auto& radiusData : s_attenuationTable)
        {
            if (glm::abs(radius - radiusData[0]) < epsilon)
            {
                _data.linear = radiusData[1];
                _data.quadratic = radiusData[2];
                return;
            }

            if (radius > radiusData[0])
            {
                minRadius = radiusData;
                continue;
            }

            if (radius < radiusData[0])
            {
                maxRadius = radiusData;

                float t = (radius - minRadius[0]) / (maxRadius[0] - minRadius[0]);

                _data.linear = math::lerp(minRadius[1], maxRadius[1], t);
                _data.quadratic = math::lerp(minRadius[2], maxRadius[2], t);
                return;
            }
        }
    }
}