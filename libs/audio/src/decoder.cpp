// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "decoder.h"

#include <yttrium/base/exceptions.h>

#if YTTRIUM_AUDIO_AULOS
#	include "formats/aulos.h"
#endif
#if YTTRIUM_AUDIO_OGGVORBIS
#	include "formats/ogg_vorbis.h"
#endif
#if YTTRIUM_AUDIO_WAV
#	include "formats/wav.h"
#endif

#include <primal/endian.hpp>

namespace Yt
{
	std::unique_ptr<AudioDecoder> AudioDecoder::create(std::unique_ptr<Source>&& source, [[maybe_unused]] bool looping)
	{
		if (!source)
			throw std::logic_error{ "Can't create AudioDecoder from an empty Source" };
#if YTTRIUM_AUDIO_AULOS || YTTRIUM_AUDIO_OGGVORBIS || YTTRIUM_AUDIO_WAV
		uint32_t signature = 0;
		if (source->read_at(0, signature))
		{
			switch (signature)
			{
			case primal::makeCC('O', 'g', 'g', 'S'):
#	if YTTRIUM_AUDIO_OGGVORBIS
				return std::make_unique<Yt::OggVorbisDecoder>(std::move(source));
#	else
				break;
#	endif
			case primal::makeCC('R', 'I', 'F', 'F'):
#	if YTTRIUM_AUDIO_WAV
				return std::make_unique<Yt::WavDecoder>(std::move(source));
#	else
				break;
#	endif
			default:
#	if YTTRIUM_AUDIO_AULOS
				return std::make_unique<Yt::AulosDecoder>(std::move(source), looping);
#	else
				break;
#	endif
			}
		}
#endif
		throw Yt::DataError{ "Unknown audio format" };
	}
}
