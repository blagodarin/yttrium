// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace Yt
{
	class KeyEvent;

	class LineEditor
	{
	public:
		void clear() noexcept;
		auto cursor() const noexcept { return _cursor; }
		void insert(std::string_view);
		bool process_key(const KeyEvent&);
		void reset(std::string&&) noexcept;
		auto selection_offset() const noexcept { return _selection_offset; }
		auto selection_size() const noexcept { return _selection_size; }
		void set_max_bytes(std::size_t) noexcept;
		auto& text() const noexcept { return _text; }

	private:
		std::size_t left_codepoint_bytes() const noexcept;
		std::size_t right_codepoint_bytes() const noexcept;

	private:
		std::string _text;
		std::size_t _cursor = 0;
		std::size_t _selection_size = 0;
		std::size_t _selection_offset = 0;
		std::size_t _max_bytes = _text.max_size();
	};
}
