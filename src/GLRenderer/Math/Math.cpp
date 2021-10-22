#include "Math.hpp"

namespace glrenderer::math {

	float lerp(float start, float end, float t)
	{
		return start * (1 - t) + end * t;
	}

}