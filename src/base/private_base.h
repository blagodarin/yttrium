#ifndef _src_base_private_base_h_
#define _src_base_private_base_h_

#include <yttrium/base.h>
#include <yttrium/memory/allocator.h>

#include <atomic>
#include <cassert>
#include <cstdlib>
#include <utility>

namespace Yttrium
{
	template <typename T>
	class PrivateBase
	{
	public:

		PrivateBase(Allocator* allocator)
			: _allocator(allocator)
			, _references(allocator ? 1 : 0)
		{
		}

		~PrivateBase()
		{
			assert(_references == 0);
		}

		PrivateBase(const PrivateBase&) = delete;
		PrivateBase& operator=(const PrivateBase&) = delete;

	public:

		static T* copy(T* object)
		{
			if (!object)
				return nullptr;
			assert(object->_allocator);
			if (object->_references++ == 0)
				std::abort(); // Tried to copy an object which is being destroyed.
			return object;
		}

		static void copy(T*& target, T* const source)
		{
			// The source's reference count should be incremented before the target's is decremented;
			// this transparently solves the (rare) problem of assigning an object to itself.
			if (source)
				++source->_references;
			if (target && !--target->_references)
				Y_DELETE(target->_allocator, target);
			target = source;
		}

		static void move(T*& target, T*& source)
		{
			if (target && !--target->_references)
				Y_DELETE(target->_allocator, target);
			target = source;
			source = nullptr;
		}

		static bool release(T* object)
		{
			Allocator* allocator = object->_allocator;
			if (allocator && !--object->_references)
			{
				Y_DELETE(allocator, object);
				return true;
			}
			return false;
		}

		static void release(T** object_ptr)
		{
			T* object = *object_ptr;
			if (object)
			{
				Allocator* allocator = object->_allocator;
				if (allocator) // Otherwise it's an object with static private data.
				{
					if (!--object->_references)
						Y_DELETE(allocator, object);
					*object_ptr = nullptr;
				}
			}
		}

	public:

		Allocator* const _allocator;

#ifndef __YTTRIUM_TEST
	private:
#else
	public:
#endif

		std::atomic_size_t _references;
	};

	template <typename T>
	class PrivateHolder
	{
	public:

		PrivateHolder()
			: _pointer(nullptr)
		{
		}

		template <typename... Args>
		PrivateHolder(Args&&... args, Allocator* allocator)
			: _pointer(Y_NEW(allocator, T)(std::forward<Args>(args)..., allocator))
		{
		}

		~PrivateHolder()
		{
			PrivateBase<T>::release(&_pointer);
		}

		T* operator->() const
		{
			return _pointer;
		}

		T* release()
		{
			const auto result = _pointer;
			_pointer = nullptr;
			return result;
		}

		template <typename U, typename... Args>
		void reset(Allocator* allocator, Args&&... args)
		{
			assert(!_pointer);
			_pointer = Y_NEW(allocator, U)(std::forward<Args>(args)..., allocator);
		}

		PrivateHolder(const PrivateHolder&) = delete;
		PrivateHolder& operator=(const PrivateHolder&) = delete;

	private:

		T* _pointer;
	};
}

#define Y_IMPLEMENT_UNIQUE(Class) \
	Class::~Class() { Private::release(&_private); } \
	Class& Class::operator=(Class&& x) noexcept { Private::move(_private, x._private); return *this; } \

#endif
