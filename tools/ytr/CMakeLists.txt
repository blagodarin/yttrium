# This file is part of the Yttrium toolkit.
# Copyright (C) Sergei Blagodarin.
# SPDX-License-Identifier: Apache-2.0

source_group("src" REGULAR_EXPRESSION ".*\\.(h|cpp)$")
add_executable(ytr
	src/main.cpp
	)
target_link_libraries(ytr PRIVATE Y_base Y_i18n Y_ion Y_main Y_storage)
seir_target(ytr FOLDER tools STATIC_RUNTIME ON)
seir_install(ytr)
