/// \file
/// \brief

#ifndef _include_yttrium_base_h_
#define _include_yttrium_base_h_

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

#endif
