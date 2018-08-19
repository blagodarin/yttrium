#ifndef _src_storage_source_h_
#define _src_storage_source_h_

#include <yttrium/storage/source.h>

namespace Yttrium
{
	std::unique_ptr<Source> create_source(const std::shared_ptr<const Buffer>&, const std::string& name);
}

#endif
