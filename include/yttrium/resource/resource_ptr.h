/// \file
/// \brief

#ifndef _include_yttrium_resource_resource_ptr_h_
#define _include_yttrium_resource_resource_ptr_h_

#include <yttrium/global.h>
#include <yttrium/resource/resource.h>

#include <utility>

namespace Yttrium
{
	class Y_API ResourcePtrBase
	{
	public:
		ResourcePtrBase() noexcept = default;
		ResourcePtrBase(const ResourcePtrBase& other) noexcept : _pointer(other._pointer) { if (_pointer) ++_pointer->_references; }
		ResourcePtrBase(ResourcePtrBase&& other) noexcept : _pointer(other._pointer) { other._pointer = nullptr; }
		~ResourcePtrBase() noexcept { if (_pointer && --_pointer->_references == 0) delete _pointer; }
		ResourcePtrBase& operator=(const ResourcePtrBase& other) noexcept;
		ResourcePtrBase& operator=(ResourcePtrBase&& other) noexcept;
		explicit operator bool() const noexcept { return _pointer; }
		bool has_another_references() const noexcept { return _pointer->_references.load() > 1; } // TODO: Hide.
	protected:
		ResourcePtrBase(Resource* pointer) noexcept : _pointer(pointer) { if (_pointer) ++_pointer->_references; }
		Resource* get() const noexcept { return _pointer; }
	private:
		Resource* _pointer = nullptr;
	};

	/// Shared pointer to a resource.
	template <typename T>
	class ResourcePtr : public ResourcePtrBase
	{
	public:
		ResourcePtr() noexcept = default;
		ResourcePtr(const ResourcePtr&) noexcept = default;
		ResourcePtr(ResourcePtr&&) noexcept = default;
		~ResourcePtr() noexcept = default;
		ResourcePtr& operator=(const ResourcePtr&) noexcept = default;
		ResourcePtr& operator=(ResourcePtr&&) noexcept = default;

		template <typename U, typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
		ResourcePtr(const ResourcePtr<U>& other) noexcept : ResourcePtrBase(other) {}

		template <typename U, typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
		ResourcePtr(ResourcePtr<U>&& other) noexcept : ResourcePtrBase(other) {}

		T* get() const noexcept { return static_cast<T*>(ResourcePtrBase::get()); }
		T* operator->() const noexcept { return get(); }
		T& operator*() const noexcept { return *get(); }

	private:
		ResourcePtr(T* pointer) noexcept : ResourcePtrBase(pointer) {}
		template <typename U, typename... Args, typename> friend ResourcePtr<U> make_resource(Args&&...);
		template <typename> friend class ResourcePtr;
	};

	///
	template <typename T, typename... Args, typename = std::enable_if_t<std::is_base_of<Resource, T>::value>>
	ResourcePtr<T> make_resource(Args&&... args)
	{
		return ResourcePtr<T>(new T(std::forward<Args>(args)...));
	}

	///
	template <typename T1, typename T2>
	bool operator==(const ResourcePtr<T1>& a, const ResourcePtr<T1>& b) noexcept { return a.get() == b.get(); }

	///
	template <typename T1, typename T2>
	bool operator!=(const ResourcePtr<T1>& a, const ResourcePtr<T1>& b) noexcept { return !(a == b); }
}

#endif
