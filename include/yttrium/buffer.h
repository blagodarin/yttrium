/// \file
/// \brief

#ifndef __Y_BUFFER_H
#define __Y_BUFFER_H

#include <yttrium/types.h>

#include <cstddef>

namespace Yttrium
{
	///
	class Y_API Buffer
	{
	public:

		///
		Buffer(Allocator* allocator = DefaultAllocator): _allocator(allocator) {}

		///
		Buffer(const Buffer& buffer, Allocator* allocator = nullptr);

		///
		explicit Buffer(size_t size, Allocator* allocator = DefaultAllocator);

		///
		~Buffer();

	public:

		///
		Allocator* allocator() const { return _allocator; }

		///
		const void* const_data() const { return _data; }

		///
		const void* const_data(size_t offset) { return static_cast<const char*>(_data) + offset; }

		///
		void* data() { return _data; }

		///
		void* data(size_t offset) { return static_cast<char*>(_data) + offset; }

		///
		const void* data() const { return _data; }

		///
		void resize(size_t size);

		///
		size_t size() const { return _size; }

		///
		void swap(Buffer&);
		void swap(Buffer&&);

	public:

		///
		bool operator ==(const Buffer& buffer) const;

		///
		bool operator !=(const Buffer& buffer) const;

	private:

		Allocator* _allocator = DefaultAllocator;
		void*      _data = nullptr;
		size_t     _size = 0;
	};
}

#endif // __Y_BUFFER_H
