#ifndef _src_storage_formats_ypq_h_
#define _src_storage_formats_ypq_h_

#include <yttrium/memory/buffer.h>
#include <yttrium/storage/package.h>
#include <yttrium/storage/writer.h>

#include <map>
#include <vector>

namespace Yttrium
{
	class YpqReader final : public PackageReader
	{
	public:
		YpqReader(std::unique_ptr<Source>&&);
		~YpqReader() override;

		const std::vector<StaticString>& names() const override { return _names; }
		std::unique_ptr<Source> open(std::size_t) const override;

	private:
		struct Entry;

		const std::shared_ptr<const Source> _source;
		Buffer _metadata_buffer;
		std::vector<StaticString> _names;
		std::vector<Entry> _entries;
		std::vector<std::pair<StaticString, StaticString>> _properties; // TODO: Map TinyStringMap into the metadata buffer.
	};

	class YpqWriter final : public PackageWriter
	{
	public:
		explicit YpqWriter(Writer&&);
		~YpqWriter() override;

		bool add(const std::string&, std::map<std::string, std::string>&&) override;
		bool commit() override;

	private:
		struct Entry;

		Writer _writer;
		std::vector<Entry> _entries;
		bool _committed = false;
		bool _finished = false;
	};
}

#endif
