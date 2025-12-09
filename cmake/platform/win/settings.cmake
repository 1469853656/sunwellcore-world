# Package overloads
set(BZIP2_LIBRARIES "bzip2")
set(ZLIB_LIBRARIES "zlib")

if ( MSVC )
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/msvc/settings.cmake)
elseif ( MINGW )
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/mingw/settings.cmake)
endif()
