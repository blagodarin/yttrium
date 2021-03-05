// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/renderer/texture.h>

#include <string_view>

namespace Yt
{
	class GuiPrivate;
	class IonReader;
	class IonToken;
	class ResourceLoader;

	void load_ion_gui(GuiPrivate&, ResourceLoader&, std::string_view path);
	bool read_ion_texture_filter(IonReader&, IonToken&, Texture2D::Filter&);
}
