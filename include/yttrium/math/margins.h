#ifndef _include_yttrium_math_margins_h_
#define _include_yttrium_math_margins_h_

#include <yttrium/math/size.h>

namespace Yttrium
{
	class Margins
	{
	public:
		int _top = 0;
		int _right = 0;
		int _bottom = 0;
		int _left = 0;

		constexpr Margins() = default;
		constexpr explicit Margins(int all) : _top{all}, _right{all}, _bottom{all}, _left{all} {}
		constexpr Margins(int top_bottom, int left_right) : _top{top_bottom}, _right{left_right}, _bottom{top_bottom}, _left{left_right} {}
		constexpr Margins(int top, int left_right, int bottom) : _top{top}, _right{left_right}, _bottom{bottom}, _left{left_right} {}
		constexpr Margins(int top, int right, int bottom, int left) : _top{top}, _right{right}, _bottom{bottom}, _left{left} {}
	};

	class MarginsF
	{
	public:
		float _top = 0;
		float _right = 0;
		float _bottom = 0;
		float _left = 0;

		constexpr MarginsF() noexcept = default;
		constexpr explicit MarginsF(float all) noexcept : _top{all}, _right{all}, _bottom{all}, _left{all} {}
		constexpr MarginsF(float top_bottom, float left_right) noexcept : _top{top_bottom}, _right{left_right}, _bottom{top_bottom}, _left{left_right} {}
		constexpr MarginsF(float top, float left_right, float bottom) noexcept : _top{top}, _right{left_right}, _bottom{bottom}, _left{left_right} {}
		constexpr MarginsF(float top, float right, float bottom, float left) noexcept : _top{top}, _right{right}, _bottom{bottom}, _left{left} {}
		constexpr explicit MarginsF(const Margins& m) noexcept : MarginsF{static_cast<float>(m._top), static_cast<float>(m._right), static_cast<float>(m._bottom), static_cast<float>(m._left)} {}
	};

	constexpr Size min_size(const Margins& m) noexcept { return {m._left + 1 + m._right, m._top + 1 + m._bottom}; }
}

#endif
