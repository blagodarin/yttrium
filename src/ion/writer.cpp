#include <yttrium/ion/writer.h>

#include <yttrium/static_string.h>
#include <yttrium/storage/writer.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <vector>

namespace Yttrium
{
	class IonWriterPrivate
	{
	public:
		IonWriterPrivate(Writer& writer, IonWriter::Formatting formatting)
			: _writer{ writer }
			, _pretty{ formatting == IonWriter::Formatting::Pretty }
		{
		}

		void add_name(const StaticString& name)
		{
			auto& entry = _stack.back();
			if (!(entry & IsObject))
				throw std::logic_error("Unexpected name");
			if (_pretty)
			{
				if ((entry & (IsRoot | AcceptsValues)) != IsRoot)
					write("\n"_s);
				write(StaticString{ _indentation });
			}
			else if (!(entry & HasValues))
				write(" "_s);
			write(name);
			entry |= AcceptsValues;
			entry &= ~HasValues;
		}

		void add_value(const StaticString& value)
		{
			auto& entry = _stack.back();
			if (!(entry & AcceptsValues))
				throw std::logic_error("Unexpected value");
			if (_pretty && entry & (IsObject | HasValues))
				write(" "_s);
			write("\""_s);
			for (auto begin = value.text(), end = begin + value.size();;)
			{
				const auto i = std::find_if(begin, end, [](char c){ return c == '\\' || c == '"'; });
				if (i != begin)
					write({ begin, static_cast<size_t>(i - begin) });
				if (i == end)
					break;
				const std::array<char, 2> sequence{ '\\', *i };
				write({ sequence.data(), sequence.size() });
				begin = i + 1;
			}
			write("\""_s);
			entry |= HasValues;
		}

		void begin_list()
		{
			auto& entry = _stack.back();
			if (!(entry & AcceptsValues))
				throw std::logic_error("Unexpected list");
			if (_pretty)
			{
				if (entry & (IsObject | HasValues))
					write(" "_s);
				_indentation.push_back('\t');
			}
			write("["_s);
			entry |= HasValues;
			_stack.emplace_back(AcceptsValues);
		}

		void begin_object()
		{
			auto& entry = _stack.back();
			if (!(entry & AcceptsValues))
				throw std::logic_error("Unexpected object");
			if (_pretty)
			{
				write("\n"_s);
				write(StaticString{ _indentation });
				_indentation.push_back('\t');
			}
			write("{"_s);
			entry |= HasValues;
			_stack.emplace_back(IsObject | HasValues);
		}

		void end_list()
		{
			if (_stack.back() & IsObject)
				throw std::logic_error("Unexpected end of list");
			if (_pretty)
				_indentation.pop_back();
			write("]"_s);
			_stack.pop_back();
			assert(!_stack.empty());
		}

		void end_object()
		{
			if ((_stack.back() & (IsObject | IsRoot)) != IsObject)
				throw std::logic_error("Unexpected end of object");
			if (_pretty)
			{
				_indentation.pop_back();
				write("\n"_s);
				write(StaticString{ _indentation });
			}
			write("}"_s);
			_stack.pop_back();
			assert(!_stack.empty());
		}

		void flush()
		{
			auto& entry = _stack.back();
			if (!(entry & IsRoot))
				throw std::logic_error("Unexpected end of file");
			if (_pretty && entry & AcceptsValues)
			{
				write("\n"_s);
				entry &= ~AcceptsValues;
			}
		}

	private:
		void write(const StaticString& string) const
		{
			if (_writer.write(string.text(), string.size()) != string.size())
				throw std::runtime_error("IonWriter output error");
		}

	private:
		enum
		{
			IsObject      = 1 << 0,
			AcceptsValues = 1 << 1,
			HasValues     = 1 << 2,
			IsRoot        = 1 << 3,
		};

		Writer& _writer;
		const bool _pretty;
		std::vector<uint8_t> _stack{ IsObject | HasValues | IsRoot };
		std::string _indentation;
	};

	IonWriter::IonWriter(Writer& writer, Formatting formatting)
		: _private{ std::make_unique<IonWriterPrivate>(writer, formatting) }
	{
	}

	IonWriter::~IonWriter() = default;

	void IonWriter::add_name(const StaticString& name)
	{
		_private->add_name(name);
	}

	void IonWriter::add_value(const StaticString& value)
	{
		_private->add_value(value);
	}

	void IonWriter::begin_list()
	{
		_private->begin_list();
	}

	void IonWriter::begin_object()
	{
		_private->begin_object();
	}

	void IonWriter::end_list()
	{
		_private->end_list();
	}

	void IonWriter::end_object()
	{
		_private->end_object();
	}

	void IonWriter::flush()
	{
		_private->flush();
	}
}
