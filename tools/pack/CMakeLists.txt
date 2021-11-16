# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(ypack
	src/main.cpp
	)
target_link_libraries(ypack PRIVATE Y_base Y_ion Y_storage Seir::main)
seir_target(ypack FOLDER tools STATIC_RUNTIME ON)
seir_install(ypack)
