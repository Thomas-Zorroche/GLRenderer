#include "VertexBuffer.hpp"

#include <glad/glad.h>
#include <iostream>


namespace glrenderer {

	VertexBuffer::VertexBuffer(const std::vector<VertexData>& vertices)
	{
		glCreateBuffers(1, &_id);
		
		glBindBuffer(GL_ARRAY_BUFFER, _id);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), &vertices[0], GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &_id);
		std::cout << "VBO deleted" << std::endl;
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