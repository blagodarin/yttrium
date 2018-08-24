#pragma once

#include <string_view>

namespace Yttrium
{
	class Buffer;
}

Yttrium::Buffer make_buffer(std::string_view text);
Yttrium::Buffer make_random_buffer(size_t size);
