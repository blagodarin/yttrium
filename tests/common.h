#ifndef __TESTS_COMMON_H
#define __TESTS_COMMON_H

#include <yttrium/memory_manager.h>

#include <boost/test/unit_test.hpp>

#define DECLARE_MEMORY_MANAGER \
	Yttrium::MemoryManager memory_manager(boost::unit_test::framework::current_test_case().p_name.value.c_str())

#endif // __TESTS_COMMON_H
