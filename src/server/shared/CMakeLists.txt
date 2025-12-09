# Copyright (C) 
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CollectSourceFiles(${CMAKE_CURRENT_SOURCE_DIR}
    shared_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders)

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

CollectIncludeDirectories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  shared_PUBLIC_INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders)

add_library(shared STATIC
  ${shared_SOURCE_FILES}
)
# cmake binary dir for revision_data.h
target_include_directories(shared PUBLIC
  ${CMAKE_BINARY_DIR}
  ${shared_PUBLIC_INCLUDES}
)

target_link_libraries(shared PUBLIC
  common_dependencies
  common
)

if (WITH_PROFILER)
  target_link_libraries(shared PUBLIC
      easy_profiler
)
endif()

# Generate precompiled header
if (USE_COREPCH)
    target_precompile_headers(shared PUBLIC PrecompiledHeaders/sharedPCH.h)
endif ()
