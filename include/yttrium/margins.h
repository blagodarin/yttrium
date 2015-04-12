/// \file
/// \brief %Margins.
/// \note %Margins[F] constructor parameter order mimics that of CSS border-image-width property.

#ifndef __Y_MARGINS_H
#define __Y_MARGINS_H

#include <yttrium/global.h>

namespace Yttrium
{
	class Size;

	/// %Margins.
	class Margins
	{
	public:

		int left;   ///< Left margin.
		int top;    ///< Top margin.
		int right;  ///< Right margin.
		int bottom; ///< Bottom margin.

		/// Initialize all margins with the same value.
		/// \param all Value for all margins.
		Margins(int all = 0)
			: left(all)
			, top(all)
			, right(all)
			, bottom(all)
		{
		}

		/// Initialize margins.
		/// \param top_and_bottom Top and bottom margin.
		/// \param left_and_right Left and right margins.
		Margins(int top_and_bottom, int left_and_right)
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
		Margins(int top, int left_and_right, int bottom)
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
		Margins(int top, int right, int bottom, int left)
			: left(left)
			, top(top)
			, right(right)
			, bottom(bottom)
		{
		}

		///
		Size min_size() const;
	};

	/// %Margins.
	class Y_API MarginsF
	{
	public:

		float left;   ///< Left margin.
		float top;    ///< Top margin.
		float right;  ///< Right margin.
		float bottom; ///< Bottom margin.

		/// Initialize all margins with the same value.
		/// \param all Value for all margins.
		MarginsF(float all = 0)
			: left(all)
			, top(all)
			, right(all)
			, bottom(all)
		{
		}

		/// Initialize margins.
		/// \param top_and_bottom Top and bottom margin.
		/// \param left_and_right Left and right margins.
		MarginsF(float top_and_bottom, float left_and_right)
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
		MarginsF(float top, float left_and_right, float bottom)
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
		MarginsF(float top, float right, float bottom, float left)
			: left(left)
			, top(top)
			, right(right)
			, bottom(bottom)
		{
		}
	};
}

#endif // __Y_MARGIN_H
