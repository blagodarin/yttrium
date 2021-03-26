// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/base/flags.h>

enum class Test
{
	One = 1 << 0,
	Two = 1 << 1,
};

using namespace Yt::Operators;

static_assert(!(Yt::Flags<Test>{} & Test::One));

static_assert(Yt::Flags<Test>{ Test::One } & Test::One);
static_assert(!(Yt::Flags<Test>{ Test::One } & Test::Two));
static_assert(!(Yt::Flags<Test>{ Test::One } & Yt::Flags<Test>{ Test::Two }));

static_assert((Test::One | Test::Two) & Test::One);
static_assert((Test::One | Test::Two) & Test::Two);

static_assert((Yt::Flags<Test>{ Test::One } | Yt::Flags<Test>{ Test::Two }) == (Test::One | Test::Two));
