#ifndef _include_yttrium_console_h_
#define _include_yttrium_console_h_

#include <yttrium/api.h>

#include <memory>

namespace Yttrium
{
	class KeyEvent;
	class RenderPass;
	class ScriptContext;

	class Y_ENGINE_API Console
	{
	public:
		explicit Console(ScriptContext&);
		~Console();

		void draw(RenderPass&) const;
		bool is_visible() const;
		bool process_key_event(const KeyEvent&);
		void set_visible(bool);

	private:
		const std::unique_ptr<class ConsolePrivate> _private;
	};
}

#endif
