#include "IndexBuffer.hpp"

#include <glad/glad.h>
#include <iostream>

namespace glrenderer {

	IndexBuffer::IndexBuffer(const std::vector<uint32_t>& indices, uint32_t count)
		: _count(count)
	{
		glCreateBuffers(1, &_id);

		glBindBuffer(GL_ARRAY_BUFFER, _id);
		glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &_id);
		std::cout << "IBO deleted" << std::endl;
	}

	void IndexBuffer::bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
	}

	void IndexBuffer::unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


}