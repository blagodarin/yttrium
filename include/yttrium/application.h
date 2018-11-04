#ifndef _include_yttrium_application_h_
#define _include_yttrium_application_h_

#include <yttrium/api.h>

#include <memory>

namespace Yttrium
{
	class Y_API Application
	{
	public:
		Application();
		~Application() noexcept;

		void run();

	private:
		const std::unique_ptr<class ApplicationPrivate> _private;
		friend ApplicationPrivate;
	};
}

#endif
