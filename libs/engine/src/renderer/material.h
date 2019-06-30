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

#pragma once

#include <yttrium/renderer/material.h>
#include <yttrium/renderer/texture.h>

#include <memory>

namespace Yttrium
{
	class RenderProgram;

	class MaterialImpl final : public Material
	{
	public:
		MaterialImpl(std::unique_ptr<RenderProgram>&&, std::shared_ptr<const Texture2D>&&, Texture2D::Filter);

		RenderProgram& program() noexcept { return *_program; }
		const RenderProgram& program() const noexcept { return *_program; }
		const Texture2D* texture() const noexcept { return _texture.get(); }
		Texture2D::Filter texture_filter() const noexcept { return _texture_filter; }

	protected:
		const std::unique_ptr<RenderProgram> _program;
		const std::shared_ptr<const Texture2D> _texture;
		const Texture2D::Filter _texture_filter;
	};
}
