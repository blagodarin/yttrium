/// \file
/// \brief %Application object.

#ifndef __Y_APPLICATION_HPP
#define __Y_APPLICATION_HPP

#include <Yttrium/allocators.hpp>

namespace Yttrium
{

///

class Y_API Application
{
public:

	///

	Application() throw();

	///

	~Application() throw();

	///
	/// \note This function is not reentrant!

	void initialize();

public:

	class Y_PRIVATE Private;

private:

	Private *_private;
};

} // namespace Yttrium

#endif // __Y_APPLICATION_HPP
