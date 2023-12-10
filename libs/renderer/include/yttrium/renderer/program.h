// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace seir
{
	class Mat4;
}

namespace Yt
{
	/// Rendering pipeline program.
	class RenderProgram
	{
	public:
		virtual ~RenderProgram() = default;

		///
		virtual void set_uniform(const std::string& name, const seir::Mat4&) = 0;
	};
}
