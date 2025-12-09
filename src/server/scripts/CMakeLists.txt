# Copyright (C) 
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# Enable precompiled headers when using the GCC compiler.

message(STATUS "SCRIPT PREPARATIONS")

if(SCRIPTS)
  CollectSourceFiles(${CMAKE_CURRENT_SOURCE_DIR}
    scripts_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders )
else()
  CollectSourceFiles(${CMAKE_CURRENT_SOURCE_DIR}/Spells
    scripts_spells_SOURCE_FILES )
  CollectSourceFiles(${CMAKE_CURRENT_SOURCE_DIR}/Commands
    scripts_commands_SOURCE_FILES )
  set(scripts_SOURCE_FILES ${scripts_spells_SOURCE_FILES} ${scripts_commands_SOURCE_FILES})
endif()

message(STATUS "SCRIPT PREPARATION COMPLETE")
message(STATUS "")


GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

add_library(scripts STATIC
  ${scripts_SOURCE_FILES}
)

target_include_directories(scripts PUBLIC
  ${CMAKE_SOURCE_DIR}/src/server/collision
  ${CMAKE_SOURCE_DIR}/src/server/collision/Management
  ${CMAKE_SOURCE_DIR}/src/server/collision/Models
  ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders
)

target_link_libraries(scripts PUBLIC shared game)
# Generate precompiled header
if (USE_SCRIPTPCH)
    target_precompile_headers(scripts PUBLIC PrecompiledHeaders/ScriptPCH.h)
endif()
