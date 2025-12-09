CollectSourceFiles(${CMAKE_CURRENT_SOURCE_DIR}
    common_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders
    ${CMAKE_CURRENT_SOURCE_DIR}/Debugging )

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

CollectIncludeDirectories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  common_PUBLIC_INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders)

# Manually set sources for Debugging directory as we don't want to include WheatyExceptionReport in shared project
# It needs to be included both in authserver and worldserver for the static global variable to be properly initialized
# and to handle crash logs on windows
add_library(common STATIC
  Debugging/Errors.cpp
  Debugging/Errors.h
  ${common_SOURCE_FILES}
)
# cmake binary dir for revision_data.h
target_include_directories(common PUBLIC
  ${CMAKE_BINARY_DIR}
  ${common_PUBLIC_INCLUDES}
)

target_link_libraries(common PUBLIC
  common_dependencies
  easy_profiler
)

# Generate precompiled header
if (USE_COREPCH)
    target_precompile_headers(common PUBLIC PrecompiledHeaders/commonPCH.h)
endif ()
