//
// Copyright 2019 Sergei Blagodarin
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

#include "material.h"

#include <yttrium/renderer/program.h>

namespace Yttrium
{
	MaterialImpl::MaterialImpl(std::unique_ptr<RenderProgram>&& program, std::shared_ptr<const Texture2D>&& texture, Texture2D::Filter texture_filter)
		: _program{ std::move(program) }
		, _texture{ std::move(texture) }
		, _texture_filter{ texture_filter }
	{
	}
}
