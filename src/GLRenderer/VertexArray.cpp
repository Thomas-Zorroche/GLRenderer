#include "VertexArray.hpp"

#include <glad/glad.h>
#include <iostream>

namespace glrenderer {

	VertexArray::VertexArray()
	{
		glGenVertexArrays(1, &_id);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &_id);
		std::cout << "VAO deleted" << std::endl;
	}

	void VertexArray::bind()
	{
		glBindVertexArray(_id);
	}

	void VertexArray::unbind()
	{
		glBindVertexArray(0);
	}

	void VertexArray::setVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		_vertexCount = vertexBuffer->getVertexCount();

		glBindVertexArray(_id);
		
		// Bind vertex buffer
		vertexBuffer->bind();

		// Set vertex layout
		const auto& layout = vertexBuffer->getLayout();
		uint32_t attributeIndex = 0;
		for (const auto& attribute : layout.getAttributes())
		{
			glEnableVertexAttribArray(attributeIndex);
			glVertexAttribPointer(attributeIndex,
				attribute.componentCount, 
				GL_FLOAT, 
				GL_FALSE, 
				layout.getStride(), 
				(void*)attribute.offset);
			
			attributeIndex++;
		}

		glBindVertexArray(0);
	}

	void VertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		_indexBuffer = indexBuffer;

		glBindVertexArray(_id);
		_indexBuffer->bind();
		glBindVertexArray(0);
	}



}