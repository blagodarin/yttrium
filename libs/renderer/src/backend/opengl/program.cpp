// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "program.h"

#include <yttrium/base/logger.h>

namespace Yt
{
	GlProgram::GlProgram(GlShaderHandle&& vertex_shader, GlShaderHandle&& fragment_shader, const GlApi& gl)
		: _vertex_shader{ std::move(vertex_shader) }
		, _fragment_shader{ std::move(fragment_shader) }
		, _program{ gl }
	{
		_program.attach(_vertex_shader.get());
		_program.attach(_fragment_shader.get());
	}

	void GlProgram::set_uniform(const std::string& name, const seir::Mat4& value)
	{
		_program.set_uniform(name.c_str(), value);
	}

	bool GlProgram::link()
	{
		if (_program.link())
			return true;
		Logger::write(_program.info_log());
		return false;
	}
}
