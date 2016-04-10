/// \file
/// \brief

#ifndef _include_yttrium_memory_object_h_
#define _include_yttrium_memory_object_h_

#include <yttrium/memory/allocator.h>

#include <atomic>
#include <type_traits>

namespace Yttrium
{
	/// An object with a reference counter.
	class Y_API Object
	{
		friend Allocator; // To make Object::~Object available.
		template <typename> friend class UniquePtr;
		template <typename> friend class SharedPtr;

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
	};

	/// Unique pointer to an Object descendant.
	template <typename T>
	class UniquePtr
	{
		template <typename> friend class UniquePtr;
		template <typename> friend class SharedPtr;
	public:

		UniquePtr() = default;

		UniquePtr(const UniquePtr&) = delete;

		UniquePtr(UniquePtr&& p)
			: _object(p._object)
		{
			p._object = nullptr;
		}

		template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
		UniquePtr(UniquePtr<U>&& p)
			: _object(p._object)
		{
			p._object = nullptr;
		}

		explicit UniquePtr(T* p)
			: _object(p)
		{
		}

		~UniquePtr()
		{
			if (_object)
				Y_DELETE(_object->_allocator, _object);
		}

		UniquePtr& operator=(const UniquePtr&) = delete;

		UniquePtr& operator=(UniquePtr&& p)
		{
			if (_object)
				Y_DELETE(_object->_allocator, _object);
			_object = p._object;
			p._object = nullptr;
			return *this;
		}

		template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
		UniquePtr& operator=(UniquePtr<U>&& p)
		{
			if (_object)
				Y_DELETE(_object->_allocator, _object);
			_object = p._object;
			p._object = nullptr;
			return *this;
		}

		explicit operator bool() const { return _object; }

		T* get() const { return static_cast<T*>(_object); }

		T* operator->() const { return get(); }

		T& operator*() const { return *get(); }

		template <typename U>
		bool operator==(const UniquePtr<U>& p) const { return _object == p._object; }

		template <typename U>
		bool operator!=(const UniquePtr<U>& p) const { return _object != p._object; }

	private:
		Object* _object = nullptr;
	};

	/// Shared pointer to an Object descendant.
	template <typename T>
	class SharedPtr
	{
		template <typename> friend class SharedPtr;
	public:

		SharedPtr() = default;

		SharedPtr(const SharedPtr& p)
			: _object(p._object)
		{
			if (_object)
				++_object->_counter;
		}

		template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
		SharedPtr(const SharedPtr<U>& p)
			: _object(p._object)
		{
			if (_object)
				++_object->_counter;
		}

		SharedPtr(SharedPtr&& p)
			: _object(p._object)
		{
			p._object = nullptr;
		}

		template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
		SharedPtr(SharedPtr<U>&& p)
			: _object(p._object)
		{
			p._object = nullptr;
		}

		explicit SharedPtr(T* p)
			: _object(p)
		{
			if (_object)
				++_object->_counter;
		}

		SharedPtr(UniquePtr<T>&& p)
			: _object(p._object)
		{
			p._object = nullptr;
		}

		template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
		SharedPtr(UniquePtr<U>&& p)
			: _object(p._object)
		{
			p._object = nullptr;
		}

		~SharedPtr()
		{
			if (_object && !--_object->_counter)
				Y_DELETE(_object->_allocator, _object);
		}

		SharedPtr& operator=(const SharedPtr& p)
		{
			if (_object && !--_object->_counter)
				Y_DELETE(_object->_allocator, _object);
			_object = p._object;
			if (_object)
				++_object->_counter;
			return *this;
		}

		SharedPtr& operator=(SharedPtr&& p)
		{
			if (_object && !--_object->_counter)
				Y_DELETE(_object->_allocator, _object);
			_object = p._object;
			p._object = nullptr;
			return *this;
		}

		template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
		SharedPtr& operator=(SharedPtr<U>&& p)
		{
			if (_object && !--_object->_counter)
				Y_DELETE(_object->_allocator, _object);
			_object = p._object;
			p._object = nullptr;
			return *this;
		}

		explicit operator bool() const { return _object; }

		T* get() const { return static_cast<T*>(_object); }

		T* operator->() const { return get(); }

		T& operator*() const { return *get(); }

		template <typename U>
		bool operator==(const SharedPtr<U>& p) const { return _object == p._object; }

		template <typename U>
		bool operator!=(const SharedPtr<U>& p) const { return _object != p._object; }

	private:
		Object* _object = nullptr;
	};
}

#endif
