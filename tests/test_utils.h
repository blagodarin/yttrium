#ifndef _tests_test_utils_h_
#define _tests_test_utils_h_

#include <yttrium/memory/buffer.h>
#include <yttrium/std/string_view.h>

inline Yttrium::Buffer make_buffer(std::string_view string)
{
	return {string.size(), string.data()};
}

#endif
