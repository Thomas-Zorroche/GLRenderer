#pragma once

#include <stdint.h>
#include <vector>

namespace glrenderer {

	struct BufferAttribute
	{
		uint32_t componentCount = 3; // 3 float numbers (X Y Z coordinates)
		uint32_t size = 3 * 4;       // 1 float = 4 bytes
		uint32_t offset = 0;

		BufferAttribute() = default;
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}

		BufferLayout(const std::vector<BufferAttribute>& attributes)
			: _attributes(attributes)
		{
			computeOffsetsAndStride();
		}

		const std::vector<BufferAttribute>& getAttributes() const { return _attributes; }
		uint32_t getStride() const { return _stride; }

		//std::vector<BufferLayout>::const_iterator begin() const { return _attributes.begin(); }
		//std::vector<BufferLayout>::const_iterator end() const { return _attributes.end(); }

	private:
		void computeOffsetsAndStride();

	private:
		std::vector<BufferAttribute> _attributes;
		// Specifies the byte offset between consecutive vertex attributes
		uint32_t _stride;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(const std::vector<float>& vertices);
		~VertexBuffer();

		void bind();
		void unbind();

		const BufferLayout& getLayout() const { return _layout; }
		void setLayout(const BufferLayout& layout) { _layout = layout; }

	private:
		uint32_t _id;
		BufferLayout _layout;
	};
}