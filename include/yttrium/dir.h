/// \file
/// \brief Directory facilities.

#ifndef _include_yttrium_dir_h_
#define _include_yttrium_dir_h_

#include <yttrium/memory/unique_ptr.h>
#include <yttrium/static_string.h>

namespace Yttrium
{
	/// Directory access class.
	class Y_API Dir
	{
	public:

		///
		class Iterator
		{
			friend Dir;

			Iterator& operator=(const Iterator&) = delete;

		public:

			Iterator(const Iterator& iterator)
				: _private(iterator._private)
			{
				const_cast<Iterator&>(iterator)._private = nullptr;
			}

			~Iterator();

		public:

			void operator++();

			StaticString operator*() const;

			bool operator!=(Iterator iterator) const;

		private:

			class Private;

			Private* _private;

			Iterator(Private* private_)
				: _private(private_)
			{
			}
		};

		///
		explicit Dir(const StaticString& name, Allocator& = *DefaultAllocator);

		///
		explicit operator bool() const noexcept { return static_cast<bool>(_private); }

		///
		Iterator begin() const;

		///
		Iterator end() const { return Iterator(nullptr); }

		///
		static bool exists(const StaticString& name);

	private:
		class Private;
		UniquePtr<Private> _private;

	public:
		~Dir();
	};
}

#endif
