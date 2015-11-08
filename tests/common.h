#ifndef _tests_common_h_
#define _tests_common_h_

#include <boost/test/unit_test.hpp>

#include <yttrium/memory/manager.h>

#define DECLARE_MEMORY_MANAGER \
	Yttrium::MemoryManager memory_manager(boost::unit_test::framework::current_test_case().p_name.value.c_str())

#endif
