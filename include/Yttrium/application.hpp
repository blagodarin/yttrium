/// \file
/// \brief

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

	Application();

	///

	~Application() throw();

public:

	class Private;

private:

	Private *_private;
};

} // namespace Yttrium

#endif // __Y_APPLICATION_HPP
