/// \file
/// \brief

#ifndef _include_yttrium_math_margins_h_
#define _include_yttrium_math_margins_h_

#include <yttrium/math/size.h>

namespace Yttrium
{
	/// Integral margins.
	class Margins
	{
	public:
		Margins() = default;
		Margins(int all) : _top{all}, _right{all}, _bottom{all}, _left{all} {}
		Margins(int top_bottom, int left_right) : _top{top_bottom}, _right{left_right}, _bottom{top_bottom}, _left{left_right} {}
		Margins(int top, int left_right, int bottom) : _top{top}, _right{left_right}, _bottom{bottom}, _left{left_right} {}
		Margins(int top, int right, int bottom, int left) : _top{top}, _right{right}, _bottom{bottom}, _left{left} {}

		int bottom() const { return _bottom; }
		int left() const { return _left; }
		int right() const { return _right; }
		int top() const { return _top; }

		///
		Size min_size() const { return {_left + 1 + _right, _top + 1 + _bottom}; }

	private:
		int _top = 0;
		int _right = 0;
		int _bottom = 0;
		int _left = 0;
	};

	/// Floating-point margins.
	class MarginsF
	{
	public:
		MarginsF() = default;
		MarginsF(float all) : _top{all}, _right{all}, _bottom{all}, _left{all} {}
		MarginsF(float top_bottom, float left_right) : _top{top_bottom}, _right{left_right}, _bottom{top_bottom}, _left{left_right} {}
		MarginsF(float top, float left_right, float bottom) : _top{top}, _right{left_right}, _bottom{bottom}, _left{left_right} {}
		MarginsF(float top, float right, float bottom, float left) : _top{top}, _right{right}, _bottom{bottom}, _left{left} {}
		explicit MarginsF(const Margins& m) : MarginsF{static_cast<float>(m.top()), static_cast<float>(m.right()), static_cast<float>(m.bottom()), static_cast<float>(m.left())} {}

		float bottom() const { return _bottom; }
		float left() const { return _left; }
		float right() const { return _right; }
		float top() const { return _top; }

	private:
		float _top = 0;
		float _right = 0;
		float _bottom = 0;
		float _left = 0;
	};
}

#endif
