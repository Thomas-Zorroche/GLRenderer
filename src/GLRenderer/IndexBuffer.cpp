#include "IndexBuffer.hpp"

#include <glad/glad.h>

namespace glrenderer {

	IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t count)
		: _count(count)
	{
		glCreateBuffers(1, &_id);

		glBindBuffer(GL_ARRAY_BUFFER, _id);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &_id);
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