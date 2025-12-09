# Copyright (C) 
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# Create game-libary

CollectSourceFiles(${CMAKE_CURRENT_SOURCE_DIR}
    game_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders )

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

CollectIncludeDirectories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  game_PUBLIC_INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders)

add_library(game STATIC
  ${game_SOURCE_FILES}
)

target_include_directories(game PUBLIC
  ${game_PUBLIC_INCLUDES}
)
target_link_libraries(game PUBLIC shared fmt)
# Generate precompiled header
if (USE_COREPCH)
  target_precompile_headers(game PUBLIC PrecompiledHeaders/gamePCH.h)
endif ()
