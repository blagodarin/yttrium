// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace Yt
{
	class Matrix4;

	/// Rendering pipeline program.
	class RenderProgram
	{
	public:
		virtual ~RenderProgram() = default;

		///
		virtual void set_uniform(const std::string& name, const Matrix4&) = 0;
	};
}
