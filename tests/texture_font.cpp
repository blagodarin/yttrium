#include <yttrium/texture_font.h>
#include <yttrium/file.h>

#include "common.h"

using namespace Yttrium;

template <typename T>
void write(File& file, const T& data)
{
	BOOST_REQUIRE(file.write(&data, sizeof data));
}

void write(File& file, const std::vector<uint8_t>& data)
{
	BOOST_REQUIRE(file.write(data.data(), data.size()));
}

BOOST_AUTO_TEST_CASE(test_texture_font_ytf1)
{
	DECLARE_MEMORY_MANAGER;

	// ASCII printable character codes.
	const uint8_t first_char = 33;
	const uint8_t last_char = 126;

	const auto char_count = last_char - first_char + 1;

	File file(File::Temporary);
	BOOST_REQUIRE(file);

	// File header.
	write(file, {'Y', 'T', 'F', '1'});

	// General information section.
	write(file, {'f', 'o', 'n', 't'});
	write(file, uint8_t{8}); // Vertical distance between consecutive lines.
	write(file, uint16_t{0}); // Horizontal offset in the texture.
	write(file, uint16_t{0}); // Vertical offset in the texture.

	// Character information section.
	write(file, {'c', 'h', 'a', 'r'});
	write(file, uint8_t{char_count}); // Number of character entries.
	for (uint8_t id = first_char; id <= last_char; ++id)
	{
		write(file, uint8_t{id}); // Character code.
		write(file, static_cast<uint16_t>((id % 16) * 8)); // Horizontal offset on the texture.
		write(file, static_cast<uint16_t>((id / 16) * 8)); // Vertical offset on the texture.
		write(file, uint8_t{8}); // Character width.
		write(file, uint8_t{8}); // Character height.
		write(file, int8_t{0}); // Horizontal offset for character output.
		write(file, int8_t{0}); // Vertical offset for character output.
		write(file, uint8_t{8}); // Base distance to the next character in the output text.
	}

	// Kerning information section.
	write(file, {'k', 'e', 'r', 'n'});
	write(file, uint16_t{char_count * char_count}); // Number of kerning entries.
	for (uint8_t id1 = first_char; id1 <= last_char; ++id1)
	{
		for (uint8_t id2 = first_char; id2 <= last_char; ++id2)
		{
			write(file, uint8_t{id1}); // First character.
			write(file, uint8_t{id2}); // Second character.
			write(file, static_cast<int8_t>((id1 + id2) & 1)); // Kerning amount.
		}
	}

	BOOST_REQUIRE(file.flush());

	TextureFont font(file.name());
	BOOST_REQUIRE(font);
}
