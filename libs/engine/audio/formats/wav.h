#pragma once

#include "../reader.h"

namespace Yttrium
{
	class WavReader final : public AudioReaderImpl
	{
	public:
		explicit WavReader(std::unique_ptr<Source>&&);

		size_t read(void*, size_t) override;
		bool seek(uint64_t) override;

	private:
		uint64_t _data_offset = 0;
	};
}
