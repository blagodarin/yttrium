add_library(ywinmain STATIC
	ywinmain.cpp
	)

if(Y_COTIRE)
	cotire(ywinmain)
endif()
