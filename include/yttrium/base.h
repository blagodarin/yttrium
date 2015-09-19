/// \file
/// \brief Base class definitions.

#ifndef _include_yttrium_base_h_
#define _include_yttrium_base_h_

namespace Yttrium
{
	///
	class Immovable
	{
	public:
		Immovable() = default;
		Immovable(const Immovable&) = delete;
		Immovable(Immovable&&) = delete;
		~Immovable() = default;
		Immovable& operator=(const Immovable&) = delete;
		Immovable& operator=(Immovable&&) = delete;
	};
}

/// Make the current class \a Class non-copyable.
#define Y_NONCOPYABLE(Class) \
	Class(const Class&) = delete; \
	Class& operator=(const Class&) = delete

///
#define Y_SHARED_PRIVATE(Class) \
	public: \
		class Private; \
		Class() = default; \
		Class(const Class&); \
		Class(Class&& class_) noexcept : _private(class_._private) { class_._private = nullptr; } \
		~Class(); \
		Class& operator=(const Class&); \
		Class& operator=(Class&&) noexcept; \
		explicit operator bool() const noexcept { return _private; } \
	protected: \
		Class(Private* private_) noexcept : _private(private_) {} \
	private: \
		Private* _private = nullptr \

///
#define Y_UNIQUE_PRIVATE(Class) \
	public: \
		class Private; \
		Class(const Class&) = delete; \
		Class(Class&& class_) noexcept : _private(class_._private) { class_._private = nullptr; } \
		~Class(); \
		Class& operator=(const Class&) = delete; \
		Class& operator=(Class&&) noexcept; \
		explicit operator bool() const noexcept { return _private; } \
	protected: \
		Class(Private* private_) noexcept : _private(private_) {} \
	private: \
		Private* _private = nullptr \

///
#define Y_MANAGER_PRIVATE(Class) \
	public: \
		class Private; \
		Class(const Class&) = delete; \
		Class(Class&&) = delete; \
		Class& operator=(const Class&) = delete; \
		Class& operator=(Class&&) = delete; \
	private: \
		Private* _private = nullptr \

#endif
