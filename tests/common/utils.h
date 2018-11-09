#pragma once

#include <string_view>

namespace Yttrium
{
	class Buffer;
}

Yttrium::Buffer make_buffer(std::string_view);
Yttrium::Buffer make_random_buffer(std::size_t);
