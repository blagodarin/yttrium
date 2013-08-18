/// \file
/// \brief Directory facilities.

#ifndef __Y_DIR_H
#define __Y_DIR_H

#include <yttrium/static_string.h>

namespace Yttrium
{

/// Directory access class.

class Y_API Dir
{
public:

	///

	class Iterator
	{
		friend Dir;

		Iterator &operator =(Iterator) noexcept;

	public:

		inline Iterator(const Iterator &iterator) noexcept;

		~Iterator() noexcept;

	public:

		void operator ++() noexcept;

		StaticString operator *() const noexcept;

		bool operator !=(Iterator iterator) const noexcept;

	private:

		class Private;

		inline Iterator(Private *private_) noexcept;

		Private *_private;
	};

public:

	///

	inline Dir() noexcept;

	///

	inline explicit Dir(const StaticString &name, Allocator *allocator = DefaultAllocator) noexcept;

	///

	Dir(const Dir &dir) noexcept;

	///

	inline ~Dir() noexcept;

public:

	///

	Iterator begin() const noexcept;

	///

	void close() noexcept;

	///

	inline Iterator end() const noexcept;

	///

	inline bool is_opened() const noexcept;

	/// Open a directory.

	bool open(const StaticString &name, Allocator *allocator = DefaultAllocator) noexcept;

public:

	///

	Dir &operator =(const Dir &dir) noexcept;

public:

	class Private;

private:

	Private *_private;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Dir::Iterator::Iterator(const Iterator &iterator) noexcept
	: _private(iterator._private)
{
	const_cast<Iterator &>(iterator)._private = nullptr;
}

Dir::Iterator::Iterator(Private *private_) noexcept
	: _private(private_)
{
}

Dir::Dir() noexcept
	: _private(nullptr)
{
}

Dir::Dir(const StaticString &name, Allocator *allocator) noexcept
	//: Dir() // TODO: Uncomment.
	: _private(nullptr)
{
	open(name, allocator);
}

Dir::~Dir() noexcept
{
	close();
}

Dir::Iterator Dir::end() const noexcept
{
	return Iterator(nullptr);
}

bool Dir::is_opened() const noexcept
{
	return _private;
}

} // namespace Yttrium

#endif // __Y_DIR_H
