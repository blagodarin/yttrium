/// \file
/// \brief

#ifndef _include_yttrium_resources_resource_ptr_h_
#define _include_yttrium_resources_resource_ptr_h_

#include <yttrium/api.h>
#include <yttrium/resources/resource.h>

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
		bool unique() const noexcept { return _pointer->_references.load() == 1; }
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
		// TODO: Can we disable copy construction/assignment for non-const Ts?
		ResourcePtr() noexcept = default;
		ResourcePtr(const ResourcePtr&) noexcept = default;
		ResourcePtr(ResourcePtr&&) noexcept = default;
		ResourcePtr(std::nullptr_t) noexcept {}
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
		template <typename U, typename, typename... Args> friend ResourcePtr<U> make_resource(Args&&...);
		template <typename> friend class ResourcePtr;
	};

	///
	template <typename T, typename = std::enable_if_t<std::is_base_of<Resource, T>::value>, typename... Args>
	ResourcePtr<T> make_resource(Args&&... args)
	{
		return ResourcePtr<T>(new T(std::forward<Args>(args)...));
	}

	///
	template <typename T1, typename T2>
	bool operator==(const ResourcePtr<T1>& a, const ResourcePtr<T2>& b) noexcept { return static_cast<const Resource*>(a.get()) == static_cast<const Resource*>(b.get()); }

	///
	template <typename T1, typename T2>
	bool operator!=(const ResourcePtr<T1>& a, const ResourcePtr<T2>& b) noexcept { return !(a == b); }
}

#endif
