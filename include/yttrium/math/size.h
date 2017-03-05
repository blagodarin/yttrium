/// \file
/// \brief

#ifndef _include_yttrium_math_size_h_
#define _include_yttrium_math_size_h_

#include <yttrium/utils.h>

#include <utility>

namespace Yttrium
{
	/// Integral 2D size.
	class Size
	{
	public:
		Size() = default;
		Size(int width, int height) : _width{width}, _height{height} {}

		int height() const { return _height; }
		int width() const { return _width; }

	private:
		int _width = 0;
		int _height = 0;
	};

	inline bool operator==(const Size& a, const Size& b) { return a.width() == b.width() && a.height() == b.height(); }
	inline bool operator!=(const Size& a, const Size& b) { return !(a == b); }

	/// Floating-point 2D size.
	class SizeF
	{
	public:
		SizeF() = default;
		SizeF(float width, float height) : _width{width}, _height{height} {}
		explicit SizeF(const Size& s) : SizeF{static_cast<float>(s.width()), static_cast<float>(s.height())} {}

		float height() const { return _height; }
		float width() const { return _width; }

		bool is_empty() const { return _width <= 0 || _height <= 0; }

		float* data() { return &_width; }
		const float* data() const { return &_width; }

	private:
		float _width = 0;
		float _height = 0;
	};

	inline SizeF operator*(const SizeF& a, float b) { return {a.width() * b, a.height() * b}; }
	inline SizeF operator*(float a, const SizeF& b) { return b * a; }
	inline SizeF operator*(const SizeF& a, const std::pair<float, float>& b) { return {a.width() * b.first, a.height() * b.second}; }
	inline SizeF operator*(const std::pair<float, float>& a, const SizeF& b) { return b * a; }

	inline SizeF operator/(const SizeF& a, float b) { return {a.width() / b, a.height() / b}; }
	inline SizeF operator/(const SizeF& a, const std::pair<float, float>& b) { return {a.width() / b.first, a.height() / b.second}; }
}

#endif
