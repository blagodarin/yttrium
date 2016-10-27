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
			const size_t line;
			const size_t position;

			Result(Status status, size_t line, size_t position) : status(status), line(line), position(position) {}
		};

		static Result parse(IonDocumentImpl&);
	};
}

#endif
