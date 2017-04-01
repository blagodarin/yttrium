/// \file
/// \brief

#ifndef _include_yttrium_storage_source_h_
#define _include_yttrium_storage_source_h_

#include <yttrium/static_string.h>
#include <yttrium/tiny_string_map.h>

namespace Yttrium
{
	class Buffer;
	class TemporaryFile;

	/// Abstract interface to immutable data.
	class Y_API Source
	{
	public:
		/// Creates a Source from a custom data range. The data must stay valid for the lifetime of the Source.
		static std::unique_ptr<Source> from(const void* data, size_t size);

		/// Creates a Source from a Buffer.
		static std::unique_ptr<Source> from(Buffer&&);

		/// Creates a Source from the specified file.
		static std::unique_ptr<Source> from(const std::string& path);

		/// Creates a Source from a temporary file.
		static std::unique_ptr<Source> from(const TemporaryFile&);

		/// Creates a Source from a part of another Source.
		static std::unique_ptr<Source> from(const std::shared_ptr<const Source>&, uint64_t base, uint64_t size);

		virtual ~Source() noexcept = default;

		/// Returns a pointer to the Source data if it resides in memory, otherwise returns nullptr.
		virtual const void* data() const noexcept { return nullptr; }

		/// Returns the name of the Source.
		StaticString name() const noexcept { return StaticString{_name}; }

		/// Retrieves a metadata property value by its name.
		StaticString property(const StaticString& name) const noexcept { return _properties.find(name); }

		/// Reads data from the source at the specified offset.
		bool read_all_at(uint64_t offset, void* data, size_t size) const { return read_at(offset, data, size) == size; }

		/// Reads the specified number of bytes of Source data at the specified offset.
		virtual size_t read_at(uint64_t, void*, size_t) const = 0;

		/// Reads data from the source at the specified offset.
		template <typename T>
		bool read_at(uint64_t offset, T& data) const { return read_at(offset, &data, sizeof data) == sizeof data; }

		/// Sets a metadata property.
		void set_property(const StaticString& name, const StaticString& value) { _properties.insert_or_assign(name, value); }

		/// Returns the size of the Source data.
		uint64_t size() const noexcept { return _size; }

		/// Creates a Buffer from the Source data.
		/// The buffer is guaranteed to be null-terminated.
		Buffer to_buffer() const;

		/// Creates a string from the Source data.
		std::string to_string() const;

	protected:
		const std::string _name;
		const uint64_t _size;

		Source(uint64_t size) noexcept : _size{size} {}
		Source(uint64_t size, const std::string& name) : _name{name}, _size{size} {}

	private:
		TinyStringMap _properties;
	};
}

#endif
