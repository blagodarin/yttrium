//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include <yttrium/utils/flags.h>

enum class Test
{
	One = 1 << 0,
	Two = 1 << 1,
};

using Yttrium::Flags;
using namespace Yttrium::Operators;

static_assert(!(Flags<Test>{} & Test::One));

static_assert(Flags<Test>{ Test::One } & Test::One);
static_assert(!(Flags<Test>{ Test::One } & Test::Two));
static_assert(!(Flags<Test>{ Test::One } & Flags<Test>{ Test::Two }));

static_assert((Test::One | Test::Two) & Test::One);
static_assert((Test::One | Test::Two) & Test::Two);

static_assert((Flags<Test>{ Test::One } | Flags<Test>{ Test::Two }) == (Test::One | Test::Two));
