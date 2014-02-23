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
	Y_NONCOPYABLE(Dir);

public:

	///

	class Iterator
	{
		friend Dir;

		Iterator& operator=(const Iterator&) = delete;

	public:

		Iterator(const Iterator& iterator) noexcept
			: _private(iterator._private)
		{
			const_cast<Iterator&>(iterator)._private = nullptr;
		}

		~Iterator() noexcept;

	public:

		void operator++() noexcept;

		StaticString operator*() const noexcept;

		bool operator!=(Iterator iterator) const noexcept;

	private:

		class Private;

		Private* _private;

		Iterator(Private *private_) noexcept
			: _private(private_)
		{
		}
	};

public:

	///

	explicit Dir(const StaticString &name, Allocator* allocator = DefaultAllocator) noexcept;

	///

	~Dir() noexcept;

public:

	///

	Iterator begin() const noexcept;

	///

	Iterator end() const noexcept
	{
		return Iterator(nullptr);
	}

	///

	bool exists() noexcept
	{
		return _private;
	}

public:

	///

	static bool exists(const StaticString &name, Allocator* allocator = DefaultAllocator) noexcept;

private:

	class Private;

	Private* _private;
};

} // namespace Yttrium

#endif // __Y_DIR_H
