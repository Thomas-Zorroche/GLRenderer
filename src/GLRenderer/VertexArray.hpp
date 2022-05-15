#pragma once

#include <stdint.h>
#include <memory>

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

namespace glrenderer {

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void bind();
		void unbind();

		void setVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);

		const std::shared_ptr<IndexBuffer>& getIndexBuffer() const { return _indexBuffer; }
		void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

		uint32_t getVertexCount() const { return _vertexCount; }

	private:
		uint32_t _id;
		std::shared_ptr<IndexBuffer> _indexBuffer = nullptr;
		uint32_t _vertexCount = 0;
	};
}