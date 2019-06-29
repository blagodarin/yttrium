//
// Copyright 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

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
			Pretty,  ///<
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
