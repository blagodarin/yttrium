#include <yttrium/flags.h>

enum class Test
{
	One = 1 << 0,
	Two = 1 << 1,
};

using Yttrium::Flags;
using namespace Yttrium::Operators;

static_assert(!(Flags<Test>{} & Test::One)
	&& Flags<Test>{ Test::One } & Test::One
	&& !(Flags<Test>{ Test::One } & Test::Two)
	&& (Test::One | Test::Two) & Test::One
	&& !(Flags<Test>{ Test::One } & Flags<Test>{ Test::Two })
	&& (Test::One | Test::Two) & Test::Two
	&& (Flags<Test>{ Test::One} | Flags<Test>{ Test::Two }) == (Test::One | Test::Two), "Flags<> type has broken");
