cmake_minimum_required( VERSION 3.28 )

set( CMAKE_CXX_COMPILER i686-w64-mingw32-g++ )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

set( CMAKE_POSITION_INDEPENDENT_CODE ON )

set_property( GLOBAL PROPERTY USE_FOLDERS ON )

if ( APPLE )

    set( MAKE_OSX_ARCHITECTURES "x86_64" )
    add_compile_options( -Wno-deprecated-declarations )

endif()

if ( MSVC )
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "/MP" )
endif()

add_subdirectory( garrysmod_common )

set( CLIENT_DLL OFF )

project( gm_unrequire VERSION 1.0.0 LANGUAGES CXX )

add_subdirectory( src )
