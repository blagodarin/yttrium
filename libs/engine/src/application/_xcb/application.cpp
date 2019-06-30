//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "application.h"

#include <yttrium/exceptions.h>

namespace Yttrium
{
	NativeApplication::NativeApplication()
	{
		int preferred_screen = 0;
		_connection.reset(::xcb_connect(nullptr, &preferred_screen));
		if (::xcb_connection_has_error(_connection))
			throw InitializationError{ "xcb_connect failed" };
		auto screen_iterator = ::xcb_setup_roots_iterator(::xcb_get_setup(_connection));
		for (; preferred_screen > 0; --preferred_screen)
			::xcb_screen_next(&screen_iterator);
		_screen = screen_iterator.data;
	}
}
