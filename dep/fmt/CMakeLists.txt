# This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

include(CheckSymbolExists)

set(strtod_l_headers stdlib.h)

if (APPLE)
  set(strtod_l_headers ${strtod_l_headers} xlocale.h)
endif ()

if(WIN32)
  check_symbol_exists(_strtod_l "${strtod_l_headers}" HAVE_STRTOD_L)
else()
  check_symbol_exists(strtod_l "${strtod_l_headers}" HAVE_STRTOD_L)
endif()

set(FMT_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/fmt/chrono.h
  ${CMAKE_CURRENT_SOURCE_DIR}/fmt/color.h
  ${CMAKE_CURRENT_SOURCE_DIR}/fmt/compile.h
  ${CMAKE_CURRENT_SOURCE_DIR}/fmt/core.h
  ${CMAKE_CURRENT_SOURCE_DIR}/fmt/format.h
  ${CMAKE_CURRENT_SOURCE_DIR}/fmt/format.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/fmt/locale.h
  ${CMAKE_CURRENT_SOURCE_DIR}/fmt/os.h
  ${CMAKE_CURRENT_SOURCE_DIR}/fmt/os.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/fmt/ostream.h
  ${CMAKE_CURRENT_SOURCE_DIR}/fmt/ranges.h
  ${CMAKE_CURRENT_SOURCE_DIR}/fmt/printf.h)

if (HAVE_OPEN)
  set(FMT_SOURCES ${FMT_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/fmt/posix.h)
endif()

add_library(fmt STATIC ${FMT_SOURCES})

target_include_directories(fmt
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(fmt
  PUBLIC
    -DFMT_USE_OVERRIDE
    -DFMT_USE_VARIADIC_TEMPLATES
    -DFMT_USE_RVALUE_REFERENCES
    -DFMT_USE_DELETED_FUNCTIONS
    -DFMT_USE_EXTERN_TEMPLATES)

set_target_properties(fmt
  PROPERTIES
    FOLDER
      Dep)
