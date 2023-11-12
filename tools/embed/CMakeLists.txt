# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(yembed
	src/main.cpp
	)
target_link_libraries(yembed PRIVATE Seir::u8main)
seir_target(yembed FOLDER tools STATIC_RUNTIME ON)
seir_install(yembed)
