/// \file
/// \brief 2D sizes.

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
		Size(int width, int height) : _width(width), _height(height) {}

		int height() const { return _height; }
		int width() const { return _width; }

	private:
		int _width = 0;
		int _height = 0;
	};

	inline bool operator==(const Size& lhs, const Size& rhs)
	{
		return lhs.width() == rhs.width() && lhs.height() == rhs.height();
	}

	inline bool operator!=(const Size& lhs, const Size& rhs)
	{
		return !(lhs == rhs);
	}

	///
	inline Size max(const Size& lhs, const Size& rhs)
	{
		return {max(lhs.width(), rhs.width()), max(lhs.height(), rhs.height())};
	}

	///
	inline Size min(const Size& lhs, const Size& rhs)
	{
		return {min(lhs.width(), rhs.width()), min(lhs.height(), rhs.height())};
	}

	/// Floating-point 2D size.
	class SizeF
	{
	public:
		SizeF() = default;
		SizeF(float width, float height) : _width(width), _height(height) {}
		explicit SizeF(const Size& size) : SizeF(size.width(), size.height()) {}

		float height() const { return _height; }
		float width() const { return _width; }

		bool is_empty() const { return _width <= 0 || _height <= 0; }

		float* data() { return &_width; }
		const float* data() const { return &_width; }

	private:
		float _width = 0;
		float _height = 0;
	};

	inline SizeF operator*(const SizeF& lhs, float rhs) { return {lhs.width() * rhs, lhs.height() * rhs}; }
	inline SizeF operator*(float lhs, const SizeF& rhs) { return {lhs * rhs.width(), lhs * rhs.height()}; }
	inline SizeF operator*(const SizeF& lhs, const std::pair<float, float>& rhs) { return {lhs.width() * rhs.first, lhs.height() * rhs.second}; }
	inline SizeF operator*(const std::pair<float, float>& lhs, const SizeF& rhs) { return {lhs.first * rhs.width(), lhs.second * rhs.height()}; }

	inline SizeF operator/(const SizeF& lhs, float rhs) { return {lhs.width() / rhs, lhs.height() / rhs}; }
	inline SizeF operator/(const SizeF& lhs, const std::pair<float, float>& rhs) { return {lhs.width() / rhs.first, lhs.height() / rhs.second}; }
}

#endif
