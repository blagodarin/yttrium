/// \file
/// \brief Smart pointers.

#ifndef _include_yttrium_pointer_h_
#define _include_yttrium_pointer_h_

#include <yttrium/memory/allocator.h>

#include <utility>
#include <type_traits>

namespace Yttrium
{
	/// Unique pointer.
	template <typename T>
	class Pointer
	{
		template <typename> friend class Pointer;
	public:

		Pointer() = default;

		Pointer(const Pointer&) = delete;

		Pointer(Pointer&& p)
			: _pointer(p._pointer)
			, _allocator(p._allocator)
		{
			p._pointer = nullptr;
		}

		template <typename U, typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
		Pointer(Pointer<U>&& p)
			: _pointer(p._pointer)
			, _allocator(p._allocator)
		{
			p._pointer = nullptr;
		}

		Pointer(T* p, Allocator& allocator)
			: _pointer(p)
			, _allocator(&allocator)
		{
		}

		~Pointer()
		{
			Y_DELETE(_allocator, _pointer);
		}

		Pointer& operator=(const Pointer&) = delete;

		Pointer& operator=(Pointer&& p)
		{
			Y_DELETE(_allocator, _pointer);
			_pointer = p._pointer;
			_allocator = p._allocator;
			p._pointer = nullptr;
			return *this;
		}

		template <typename U, typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
		Pointer& operator=(Pointer<U>&& p)
		{
			Y_DELETE(_allocator, _pointer);
			_pointer = p._pointer;
			_allocator = p._allocator;
			p._pointer = nullptr;
			return *this;
		}

		explicit operator bool() const { return _pointer; }

		T* get() const { return _pointer; }

		T* operator->() const { return _pointer; }

		T& operator*() const { return *_pointer; }

		template <typename U>
		bool operator==(const Pointer<U>& p) const { return _pointer == p._pointer; }

		template <typename U>
		bool operator!=(const Pointer<U>& p) const { return _pointer != p._pointer; }

	private:
		T* _pointer = nullptr;
		Allocator* _allocator = nullptr;
	};

	///
	template <typename T, typename... Args>
	Pointer<T> make_pointer(Allocator& allocator, Args&&... args)
	{
		return Pointer<T>(Y_NEW(&allocator, T)(std::forward<Args>(args)...), allocator);
	}
}

#endif
