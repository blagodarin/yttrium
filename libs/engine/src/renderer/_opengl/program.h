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

#include <yttrium/renderer/program.h>

#include "wrappers.h"

namespace Yttrium
{
	class GlProgram final : public RenderProgram
	{
	public:
		GlProgram(GlShaderHandle&& vertex_shader, GlShaderHandle&& fragment_shader, const GlApi&);

		void set_uniform(const std::string&, const Matrix4&) override;

		GLuint handle() const { return _program.get(); }
		bool link();

	private:
		const GlShaderHandle _vertex_shader;
		const GlShaderHandle _fragment_shader;
		const GlProgramHandle _program;
	};
}
