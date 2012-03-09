/// \file
/// \brief %Application object.

#ifndef __Y_APPLICATION_HPP
#define __Y_APPLICATION_HPP

#include <Yttrium/log_manager.hpp>

namespace Yttrium
{

///

class Y_API Application
{
public:

	///

	Application() noexcept;

	///

	~Application() noexcept;

	///

	bool initialize_audio(const StaticString &device = StaticString());

	///
	/// \note The log manager can be retrieved before the Application initialization.

	LogManager log_manager() noexcept;

public:

	class Private;

private:

	Private *_private;
};

} // namespace Yttrium

#endif // __Y_APPLICATION_HPP
