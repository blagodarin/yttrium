#ifndef _src_ion_parser_h_
#define _src_ion_parser_h_

#include <cstddef>

namespace Yttrium
{
	class IonDocumentImpl;

	class IonParser
	{
	public:
		enum class Status
		{
			Ok,
			Error,
		};

		struct Result
		{
			const Status status;
			const std::size_t line;
			const std::size_t position;

			Result(Status status_, std::size_t line_, std::size_t position_) : status(status_), line(line_), position(position_) {}
		};

		static Result parse(IonDocumentImpl&);
	};
}

#endif
