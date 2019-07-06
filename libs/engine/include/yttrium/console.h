//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#pragma once

#include <yttrium/api.h>

#include <memory>

namespace Yttrium
{
	class KeyEvent;
	class RenderPass;
	class ScriptContext;

	class Y_ENGINE_API Console
	{
	public:
		explicit Console(ScriptContext&);
		~Console();

		void draw(RenderPass&) const;
		bool is_visible() const;
		bool process_key_event(const KeyEvent&);
		void set_visible(bool);

	private:
		const std::unique_ptr<class ConsolePrivate> _private;
	};
}