/// \file
/// \brief

#ifndef _include_yttrium_resources_resource_h_
#define _include_yttrium_resources_resource_h_

#include <yttrium/api.h>

#include <atomic>
#include <cstddef>

namespace Yttrium
{
	/// Base class for all resources.
	class Y_API Resource
	{
	public:
		Resource(const Resource&) = delete;
		Resource& operator=(const Resource&) = delete;

	protected:
		Resource() = default;
		virtual ~Resource() = default;

	private:
		std::atomic<size_t> _references{ 0 };
		friend class ResourcePtrBase;
	};
}

#endif
