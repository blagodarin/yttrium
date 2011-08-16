#include "private.hpp"

namespace Yttrium
{

bool    Application::Private::_exists = false;
uint8_t Application::Private::_buffer[sizeof(Private)] = {};

} // namespace Yttrium
