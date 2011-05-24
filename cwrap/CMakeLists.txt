

add_library(cwrap SHARED
		capmode.c
		open.c
	)

add_executable(cwraptest test.c)

find_library(CAP_LIB capsicum)
target_link_libraries(cwrap ${CAP_LIB})
target_link_libraries(cwraptest cwrap)

include(CheckLibraryExists)
check_library_exists(capsicum lch_start "" HAVE_CAP_LIB)

