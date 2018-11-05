#ifndef _include_yttrium_ion_writer_h_
#define _include_yttrium_ion_writer_h_

#include <yttrium/api.h>

#include <memory>
#include <string_view>

namespace Yttrium
{
	class Writer;

	///
	/// Member functions may throw std::logic_error if the requested value or structure is not representable in ION.
	class Y_CORE_API IonWriter
	{
	public:
		///
		enum class Formatting
		{
			Pretty, ///<
			Compact, ///<
		};

		///
		IonWriter(Writer&, Formatting);

		///
		~IonWriter();

		///
		void add_name(std::string_view);

		///
		void add_value(std::string_view);

		///
		void begin_list();

		///
		void begin_object();

		///
		void end_list();

		///
		void end_object();

		///
		void flush();

	private:
		const std::unique_ptr<class IonWriterPrivate> _private;
	};
}

#endif
