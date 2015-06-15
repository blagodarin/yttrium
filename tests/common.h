#ifndef __TESTS_COMMON_H
#define __TESTS_COMMON_H

#include <boost/test/unit_test.hpp>

#include <yttrium/memory_manager.h>

#define DECLARE_MEMORY_MANAGER \
	Yttrium::MemoryManager memory_manager(boost::unit_test::framework::current_test_case().p_name.value.c_str())

#endif // __TESTS_COMMON_H
