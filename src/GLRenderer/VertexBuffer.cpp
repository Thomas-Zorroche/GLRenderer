#include "VertexBuffer.hpp"

#include <glad/glad.h>

namespace glrenderer {

	VertexBuffer::VertexBuffer(float* vertices, uint32_t size)
	{
		glCreateBuffers(1, &_id);
		
		glBindBuffer(GL_ARRAY_BUFFER, _id);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &_id);
	}

	void VertexBuffer::bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, _id);
	}

	void VertexBuffer::unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void BufferLayout::computeOffsetsAndStride()
	{
		_stride = 0;
		size_t offset = 0;
		for (auto& attribute : _attributes)
		{
			attribute.offset = offset;
			offset += attribute.size;
			_stride += attribute.size;
		}
	}



}