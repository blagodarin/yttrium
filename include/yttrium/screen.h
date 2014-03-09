/// \file
/// \brief

#ifndef __Y_SCREEN_H
#define __Y_SCREEN_H

#include <yttrium/types.h>

namespace Yttrium
{

class Window;

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

///

class Y_API Screen
{
	friend Window;

public:

	enum ModeType
	{
		CurrentMode,
		DefaultMode,
	};

public:

	///

	Screen() noexcept
		: _private(nullptr)
	{
	}

	///

	Screen(Allocator *allocator) noexcept
		: Screen()
	{
		open(allocator);
	}

	///

	Screen(const Screen &screen) noexcept;

	///

	~Screen() noexcept
	{
		close();
	}

public:

	///

	void close() noexcept;

	///

	bool is_opened() const noexcept
	{
		return _private;
	}

	///

	ScreenMode mode(ModeType type = CurrentMode) noexcept;

	///

	bool open(Allocator *allocator = DefaultAllocator) noexcept;

public:

	///

	Screen &operator =(const Screen &screen) noexcept;

private:

	class Private;

	Private *_private;
};

} // namespace Yttrium

#endif // __Y_SCREEN_H
