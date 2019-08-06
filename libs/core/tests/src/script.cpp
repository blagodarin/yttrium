//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#include <yttrium/script/context.h>
#include <yttrium/script/value.h>

#include <catch2/catch.hpp>

TEST_CASE("script.context.find")
{
	Yt::ScriptContext parent;
	Yt::ScriptContext child{ &parent };
	Yt::ScriptContext grandchild{ &child };

	CHECK(&parent == &parent.root());
	CHECK(&parent == &child.root());
	CHECK(&parent == &grandchild.root());

	CHECK(!parent.find("parent"));
	CHECK(!child.find("parent"));
	CHECK(!grandchild.find("parent"));

	parent.set("parent", "parent value");
	REQUIRE(parent.find("parent"));
	REQUIRE(child.find("parent"));
	REQUIRE(grandchild.find("parent"));
	CHECK(parent.find("parent")->string() == "parent value");
	CHECK(child.find("parent")->string() == "parent value");
	CHECK(grandchild.find("parent")->string() == "parent value");

	child.set("child", "child value");
	CHECK(!parent.find("child"));
	REQUIRE(child.find("child"));
	REQUIRE(grandchild.find("child"));
	CHECK(child.find("child")->string() == "child value");
	CHECK(grandchild.find("child")->string() == "child value");

	child.set("parent", "modified value");
	REQUIRE(parent.find("parent"));
	REQUIRE(child.find("parent"));
	REQUIRE(grandchild.find("parent"));
	CHECK(parent.find("parent")->string() == "parent value");
	CHECK(child.find("parent")->string() == "modified value");
	CHECK(grandchild.find("parent")->string() == "modified value");
}

TEST_CASE("script.context.int")
{
	Yt::ScriptContext context;
	CHECK(context.get_int("int", 7) == 7);

	context.set("int", 42);
	REQUIRE(context.find("int"));
	CHECK(context.find("int")->type() == Yt::ScriptValue::Type::Literal);
	CHECK(context.find("int")->string() == "42");
	CHECK(context.find("int")->to_int() == 42);
	CHECK(context.get_int("int", 7) == 42);

	context.set("int", 43);
	CHECK(context.find("int")->string() == "43");
	CHECK(context.get_int("int", 7) == 43);
}

TEST_CASE("script.context.string")
{
	Yt::ScriptContext context;

	context.set("string", "value");
	REQUIRE(context.find("string"));
	CHECK(context.find("string")->type() == Yt::ScriptValue::Type::String);
	CHECK(context.find("string")->string() == "value");
	CHECK(context.find("string")->to_int() == 0);

	context.set("string", "new value");
	CHECK(context.find("string")->string() == "new value");
}

TEST_CASE("script.context.substitute")
{
	std::string text;

	Yt::ScriptContext context;
	context.substitute(text, "Hi, I'm {name}! What's up?");
	CHECK(text == "Hi, I'm ! What's up?");

	context.set("name", "Yttrium");
	context.substitute(text, "Hi, I'm {name}! What's up?");
	CHECK(text == "Hi, I'm Yttrium! What's up?");

	context.substitute(text, "Hi, I'm {name");
	CHECK(text == "Hi, I'm ");
}
