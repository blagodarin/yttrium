/// \file
/// \brief Shared pointer.

#ifndef _include_yttrium_memory_shared_ptr_h_
#define _include_yttrium_memory_shared_ptr_h_

#include <yttrium/memory/unique_ptr.h>

#include <atomic>

namespace Yttrium
{
	/// An object with a reference counter.
	class Y_API Shared
	{
	public:
		Shared(const Shared&) = delete;
		Shared& operator=(const Shared&) = delete;

	protected:
		Shared() = default;
		virtual ~Shared() = default;

	private:
		std::atomic<size_t> _counter{0};
		template <typename> friend class SharedPtr;
	};

	/// %Shared pointer to Shared descendant.
	template <typename T>
	class SharedPtr
	{
	public:

		SharedPtr() = default;

		SharedPtr(const SharedPtr& other) noexcept
			: _allocation(other._allocation.allocator(), other._allocation.get())
		{
			attach();
		}

		template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
		SharedPtr(const SharedPtr<U>& other) noexcept
			: _allocation(other._allocation.allocator(), other._allocation.get())
		{
			attach();
		}

		SharedPtr(SharedPtr&& other) noexcept
			: _allocation(std::move(other._allocation))
		{
		}

		template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
		SharedPtr(SharedPtr<U>&& other) noexcept
			: _allocation(std::move(other._allocation))
		{
		}

		template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
		SharedPtr(UniquePtr<U>&& other) noexcept
			: _allocation(std::move(other._allocation))
		{
			attach();
		}

		SharedPtr(Allocator& allocator, T* pointer) noexcept
			: _allocation(allocator, pointer)
		{
			attach();
		}

		~SharedPtr()
		{
			detach();
		}

		SharedPtr& operator=(const SharedPtr& other) noexcept
		{
			detach();
			_allocation = Allocation<Shared>(other._allocation.allocator(), other._allocation.get());
			attach();
			return *this;
		}

		SharedPtr& operator=(SharedPtr&& other) noexcept
		{
			detach();
			_allocation = std::move(other._allocation);
			return *this;
		}

		template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
		SharedPtr& operator=(SharedPtr<U>&& other) noexcept
		{
			detach();
			_allocation = std::move(other._allocation);
			return *this;
		}

		explicit operator bool() const noexcept { return static_cast<bool>(_allocation); }

		Allocator& allocator() const { return _allocation.allocator(); }
		T* get() const noexcept { return static_cast<T*>(_allocation.get()); }
		T* operator->() const noexcept { return get(); }
		T& operator*() const noexcept { return *get(); }

	private:
		void attach() noexcept
		{
			if (_allocation)
				++_allocation.get()->_counter;
		}

		void detach() noexcept
		{
			if (_allocation)
			{
				if (--_allocation.get()->_counter > 0)
					_allocation.release();
				else
					_allocation.get()->~Shared();
			}
		}

	private:
		Allocation<Shared> _allocation;
		SharedPtr(Allocation<T>&& allocation) noexcept : _allocation(std::move(allocation)) { attach(); }
		template <typename U, typename... Args, typename> friend SharedPtr<U> make_shared(Allocator&, Args&&...);
		template <typename> friend class SharedPtr;
	};

	template <typename T, typename... Args, typename = std::enable_if_t<std::is_base_of<Shared, T>::value>>
	SharedPtr<T> make_shared(Allocator& allocator, Args&&... args)
	{
		Allocation<T> allocation(allocator);
		new(allocation.get()) T(std::forward<Args>(args)...);
		return SharedPtr<T>(std::move(allocation));
	}

	template <typename T1, typename T2>
	bool operator==(const SharedPtr<T1>& a, const SharedPtr<T1>& b) { return a.get() == b.get(); }

	template <typename T1, typename T2>
	bool operator!=(const SharedPtr<T1>& a, const SharedPtr<T1>& b) { return !(a == b); }
}

#endif
