#pragma once

#include <stdint.h>
#include <vector>

namespace glrenderer {

	class IndexBuffer
	{
	public:
		IndexBuffer(const std::vector<uint32_t>& indices , uint32_t count);
		~IndexBuffer();

		void bind();
		void unbind();

		uint32_t getCount() const { return _count; }


	private:
		uint32_t _id;
		uint32_t _count;

	};
}