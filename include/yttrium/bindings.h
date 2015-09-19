/// \file
/// \brief Input key bindings.

#ifndef __Y_BINDINGS_H
#define __Y_BINDINGS_H

#include <yttrium/key.h>
#include <yttrium/script/code.h>
#include <yttrium/string.h>

#include <utility>

namespace Yttrium
{
	///
	class Y_API Bindings
	{
	public:

		///
		class Iterator
		{
		public:
			bool operator!=(Iterator iterator) const { return _index != iterator._index; }
			std::pair<StaticString, StaticString> operator*() const;
			void operator++();
		private:
			friend Bindings;
			const Bindings& _bindings;
			size_t _index;
			Iterator(const Bindings& bindings, size_t index): _bindings(bindings), _index(index) {}
		};
		friend Iterator;

		///
		Bindings(Allocator* allocator = DefaultAllocator);

		///
		Iterator begin() const;

		///
		void bind(Key key, const StaticString& action);

		///
		bool bind(const StaticString &name, const StaticString& action);

		///
		void bind_default(Key key, const StaticString& action);

		///
		bool bind_default(const StaticString& name, const StaticString& action);

		///
		bool call(Key key, ScriptCode::ExecutionMode mode = ScriptCode::Do);

		///
		void clear();

		///
		Iterator end() const;

		///
		void unbind(Key key);

		///
		bool unbind(const StaticString &name);

	private:
		Y_UNIQUE_PRIVATE(Bindings);
	};
}

#endif
