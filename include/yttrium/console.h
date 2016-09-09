/// \file
/// \brief

#ifndef _include_yttrium_console_h_
#define _include_yttrium_console_h_

#include <yttrium/memory/unique_ptr.h>

namespace Yttrium
{
	class KeyEvent;
	class Renderer;
	class ScriptContext;

	///
	class Y_API Console
	{
	public:
		///
		Console(ScriptContext&, Allocator& = *DefaultAllocator);

		///
		bool is_visible() const;

		///
		bool process_key_event(const KeyEvent&);

		///
		void render(Renderer&) const;

		///
		void set_visible(bool);

	private:
		const UniquePtr<class ConsolePrivate> _private;

	public:
		~Console();
	};
}

#endif
