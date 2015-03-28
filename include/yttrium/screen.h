/// \file
/// \brief %Screen management.

#ifndef __Y_SCREEN_H
#define __Y_SCREEN_H

#include <yttrium/pointer.h>

namespace Yttrium
{

///
struct ScreenMode
{
	int      width;          ///<
	int      height;         ///<
	unsigned bits_per_pixel; ///<
	unsigned frequency;      ///<

	///
	ScreenMode()
		: width(0)
		, height(0)
		, bits_per_pixel(0)
		, frequency(0)
	{
	}
};

class Screen;

/// Screen pointer.
typedef Pointer<Screen> ScreenPtr;

///
class Y_API Screen: public Pointable
{
public:

	///
	enum ModeType
	{
		CurrentMode, ///<
		DefaultMode, ///<
	};

public:

	~Screen() = default;

	///
	virtual ScreenMode mode(ModeType type = CurrentMode) = 0;

public:

	///
	static ScreenPtr open(Allocator* allocator = DefaultAllocator);

protected:

	Screen(Allocator* allocator): Pointable(allocator) {}
};

} // namespace Yttrium

#endif // __Y_SCREEN_H
