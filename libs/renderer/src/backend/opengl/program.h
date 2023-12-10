// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/renderer/program.h>

#include "wrappers.h"

namespace Yt
{
	class GlProgram final : public RenderProgram
	{
	public:
		GlProgram(GlShaderHandle&& vertex_shader, GlShaderHandle&& fragment_shader, const GlApi&);

		void set_uniform(const std::string&, const seir::Mat4&) override;

		GLuint handle() const { return _program.get(); }
		bool link();

	private:
		const GlShaderHandle _vertex_shader;
		const GlShaderHandle _fragment_shader;
		const GlProgramHandle _program;
	};
}
