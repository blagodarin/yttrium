/// \file
/// \brief Margins.

#ifndef __Y_MARGINS_H
#define __Y_MARGINS_H

#include <yttrium/global.h>

namespace Yttrium
{

/// %Margins.
/// \note %Margins constructor parameter order mimics that of CSS border-image-width property.

template <typename T>
struct Margins
{
	T left;   ///< Left margin.
	T top;    ///< Top margin.
	T right;  ///< Right margin.
	T bottom; ///< Bottom margin.

	/// Initialize all margins with the same value.
	/// \param margin Value for all margins.

	Margins(T margin = 0) noexcept
		: left(margin)
		, top(margin)
		, right(margin)
		, bottom(margin)
	{
	}

	/// Initialize margins.
	/// \param top_and_bottom Top and bottom margin.
	/// \param left_and_right Left and right margins.

	Margins(T top_and_bottom, T left_and_right) noexcept
		: left(left_and_right)
		, top(top_and_bottom)
		, right(left_and_right)
		, bottom(top_and_bottom)
	{
	}

	/// Initialize margins.
	/// \param top Top margin.
	/// \param left_and_right Left and right margins.
	/// \param bottom Bottom margin.

	Margins(T top, T left_and_right, T bottom) noexcept
		: left(left_and_right)
		, top(top)
		, right(left_and_right)
		, bottom(bottom)
	{
	}

	/// Initialize margins.
	/// \param top Top margin.
	/// \param right Right margin.
	/// \param bottom Bottom margin.
	/// \param left Left margin.

	Margins(T top, T right, T bottom, T left) noexcept
		: left(left)
		, top(top)
		, right(right)
		, bottom(bottom)
	{
	}

	///

	template <typename U>
	Margins(const Margins<U> &margins) noexcept
		: left(margins.left)
		, top(margins.top)
		, right(margins.right)
		, bottom(margins.bottom)
	{
	}

	///

	bool is_empty() const noexcept
	{
		return !(left || top || right || bottom);
	}
};

/// Margins of <tt>int</tt>s.

typedef Margins<int> MarginsI;

} // namespace Yttrium

#endif // __Y_MARGIN_H
