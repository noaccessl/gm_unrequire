add_library( unrequire SHARED

	main.cpp

)

target_link_libraries( unrequire PUBLIC

	gmod::common

)

set_gmod_suffix_prefix( unrequire )
