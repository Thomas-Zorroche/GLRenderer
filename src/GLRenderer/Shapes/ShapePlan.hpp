#pragma once

#include <vector>
#include <stdint.h>

namespace {

	struct ShapePlan
	{
		std::vector<float> vertices = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f,
		};

		std::vector<uint32_t> indices = {
			0, 1, 2,
			2, 3, 0
		};
	};
}