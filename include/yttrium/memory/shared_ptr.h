/// \file
/// \brief Shared pointer.

#ifndef _include_yttrium_memory_shared_ptr_h_
#define _include_yttrium_memory_shared_ptr_h_

#include <yttrium/memory/unique_ptr.h>

#include <atomic>

namespace Yttrium
{
	/// An object with a reference counter.
	class Y_API Object
	{
	public:
		Object() = delete;
		Object(const Object&) = delete;
		Object(Object&&) = delete;
		Object& operator=(const Object&) = delete;
		Object& operator=(Object&&) = delete;

		///
		Allocator* allocator() const { return _allocator; }

	protected:
		explicit Object(Allocator* allocator) : _allocator(allocator) {}
		virtual ~Object() = default;

	private:
		Allocator* const _allocator;
		std::atomic<size_t> _counter{0};
		template <typename> friend class SharedPtr;
	};

	/// Shared pointer to an Object descendant.
	template <typename T>
	class SharedPtr
	{
	public:

		SharedPtr() = default;

		SharedPtr(const SharedPtr& p) noexcept
			: _object(p._object)
		{
			if (_object)
				++_object->_counter;
		}

		template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
		SharedPtr(const SharedPtr<U>& p) noexcept
			: _object(p._object)
		{
			if (_object)
				++_object->_counter;
		}

		SharedPtr(SharedPtr&& p) noexcept
			: _object(p._object)
		{
			p._object = nullptr;
		}

		template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
		SharedPtr(SharedPtr<U>&& p) noexcept
			: _object(p._object)
		{
			p._object = nullptr;
		}

		SharedPtr(UniquePtr<T>&& p) noexcept
			: _object(p._allocation.release())
		{
		}

		template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
		SharedPtr(UniquePtr<U>&& p) noexcept
			: _object(p._allocation.release())
		{
		}

		explicit SharedPtr(T* p) noexcept
			: _object(p)
		{
			if (_object)
				++_object->_counter;
		}

		~SharedPtr()
		{
			detach();
		}

		SharedPtr& operator=(const SharedPtr& p) noexcept
		{
			detach();
			_object = p._object;
			if (_object)
				++_object->_counter;
			return *this;
		}

		SharedPtr& operator=(SharedPtr&& p) noexcept
		{
			detach();
			_object = p._object;
			p._object = nullptr;
			return *this;
		}

		template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
		SharedPtr& operator=(SharedPtr<U>&& p) noexcept
		{
			detach();
			_object = p._object;
			p._object = nullptr;
			return *this;
		}

		explicit operator bool() const noexcept { return _object; }

		T* get() const noexcept { return static_cast<T*>(_object); }

		T* operator->() const noexcept { return get(); }

		T& operator*() const noexcept { return *get(); }

	private:
		void detach() noexcept
		{
			if (_object && !--_object->_counter)
			{
				_object->~Object();
				_object->_allocator->deallocate(_object);
			}
		}

	private:
		Object* _object = nullptr;
		template <typename> friend class SharedPtr;
	};

	template <typename T, typename... Args>
	SharedPtr<T> make_shared(Allocator& allocator, Args&&... args)
	{
		Allocation<T> allocation(allocator);
		new(allocation.get()) T(std::forward<Args>(args)...);
		return SharedPtr<T>(allocation.release());
	}

	template <typename T1, typename T2>
	bool operator==(const SharedPtr<T1>& a, const SharedPtr<T1>& b) { return a.get() == b.get(); }

	template <typename T1, typename T2>
	bool operator!=(const SharedPtr<T1>& a, const SharedPtr<T1>& b) { return !(a == b); }
}

#endif
