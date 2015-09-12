/// \file
/// \brief GUI.

#ifndef __Y_GUI_H
#define __Y_GUI_H

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
		virtual bool has_scene(const StaticString& name) const = 0;

		///
		virtual bool load(const StaticString& filename) = 0;

		///
		virtual bool pop_scenes(size_t count) = 0;

		///
		virtual bool push_scene(const StaticString& name) = 0;
	};
}

#endif
