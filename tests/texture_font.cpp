#include <yttrium/gui/texture_font.h>
#include <yttrium/resources/resource_ptr.h>
#include <yttrium/static_string.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/temporary_file.h>
#include <yttrium/storage/writer.h>

#include <boost/test/unit_test.hpp>

using namespace Yttrium;

template <typename T>
void write(Writer& writer, const T& data)
{
	BOOST_REQUIRE(writer.write_all(&data, sizeof data));
}

void write(Writer& writer, const std::vector<uint8_t>& data)
{
	BOOST_REQUIRE(writer.write_all(data.data(), data.size()));
}

BOOST_AUTO_TEST_CASE(test_texture_font_ytf1)
{
	// ASCII printable character codes.
	const uint8_t first_char = 33;
	const uint8_t last_char = 126;

	const auto char_count = last_char - first_char + 1;

	TemporaryFile file;
	{
		Writer writer(file);

		// File header.
		write(writer, {'Y', 'T', 'F', '1'});

		// General information section.
		write(writer, {'f', 'o', 'n', 't'});
		write(writer, uint8_t{8}); // Vertical distance between consecutive lines.
		write(writer, uint16_t{0}); // Horizontal offset in the texture.
		write(writer, uint16_t{0}); // Vertical offset in the texture.

		// Character information section.
		write(writer, {'c', 'h', 'a', 'r'});
		write(writer, uint8_t{char_count}); // Number of character entries.
		for (uint8_t id = first_char; id <= last_char; ++id)
		{
			write(writer, uint8_t{id}); // Character code.
			write(writer, static_cast<uint16_t>((id % 16) * 8)); // Horizontal offset on the texture.
			write(writer, static_cast<uint16_t>((id / 16) * 8)); // Vertical offset on the texture.
			write(writer, uint8_t{8}); // Character width.
			write(writer, uint8_t{8}); // Character height.
			write(writer, int8_t{0}); // Horizontal offset for character output.
			write(writer, int8_t{0}); // Vertical offset for character output.
			write(writer, uint8_t{8}); // Base distance to the next character in the output text.
		}

		// Kerning information section.
		write(writer, {'k', 'e', 'r', 'n'});
		write(writer, uint16_t{char_count * char_count}); // Number of kerning entries.
		for (uint8_t id1 = first_char; id1 <= last_char; ++id1)
		{
			for (uint8_t id2 = first_char; id2 <= last_char; ++id2)
			{
				write(writer, uint8_t{id1}); // First character.
				write(writer, uint8_t{id2}); // Second character.
				write(writer, static_cast<int8_t>((id1 + id2) & 1)); // Kerning amount.
			}
		}
	}

	const auto font = TextureFont::open(Reader(file));
	BOOST_REQUIRE(font);
}
