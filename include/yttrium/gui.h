/// \file
/// \brief GUI.

#ifndef _include_yttrium_gui_h_
#define _include_yttrium_gui_h_

#include <cstddef>

namespace Yttrium
{
	class StaticString;

	/// GUI manager.
	class Gui
	{
	public:

		virtual ~Gui() = default;

		///
		virtual void clear() = 0;

		///
		virtual void dump(const StaticString& filename) const = 0;

		///
		virtual bool has_layer(const StaticString& name) const = 0;

		///
		virtual bool load(const StaticString& filename) = 0;

		///
		virtual bool pop_layers(size_t count) = 0;

		///
		virtual bool push_layer(const StaticString& name) = 0;
	};
}

#endif
