﻿#include "PointLight.hpp"

#include <iostream>

namespace glrenderer {

    /*
    * Attenuation values per radius
    * radius  |  Linear  |  Quadratic
    */
    const std::unordered_map<int, std::pair<float, float>> PointLight::_attenuationTable = {
        {7, {0.7, 1.8} },
        {13, {0.35, 0.44} },
        {20, {0.22, 0.2} },
        {32, {0.14, 0.07} },
        {50, {0.09, 0.032} },
        {65, {0.07, 0.017} },
        {100, {0.045, 0.075} },
        {160, {0.027, 0.028} },
        {200, {0.022, 0.019} },
        {325, {0.014, 0.007} },
        {600, {0.007, 0.0002} },
    };

	PointLight::PointLight(const glm::vec3& color, float intensity)
		: BaseLight(LightType::Point, color, intensity)
	{
        updateAttenuationValues();
	}

	void PointLight::updateAttenuationValues()
	{
        auto it = _attenuationTable.find(_radius);
        if (it == _attenuationTable.end())
        {
            std::cout << "Wrong radius size, be sure to check the table in PointLight.cpp and choose an available radius." << std::endl;

            // Default point light attenuation values
            _radius = 20.0;
            _linear = 0.22;
            _quadratic = 0.2;
        }
        else
        {
            _linear = it->second.first;
            _quadratic = it->second.second;
        }
	}


}