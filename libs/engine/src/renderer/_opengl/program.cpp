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

#include "program.h"

#ifndef NDEBUG
#	include <iostream>
#endif

namespace Yttrium
{
	GlProgram::GlProgram(GlShaderHandle&& vertex_shader, GlShaderHandle&& fragment_shader, const GlApi& gl)
		: _vertex_shader{ std::move(vertex_shader) }
		, _fragment_shader{ std::move(fragment_shader) }
		, _program{ gl }
	{
		_program.attach(_vertex_shader.get());
		_program.attach(_fragment_shader.get());
	}

	void GlProgram::set_uniform(const std::string& name, const Matrix4& value)
	{
		_program.set_uniform(name.c_str(), value);
	}

	bool GlProgram::link()
	{
		if (_program.link())
			return true;
#ifndef NDEBUG
		std::cerr << _program.info_log() << "\n";
#endif
		return false;
	}
}
