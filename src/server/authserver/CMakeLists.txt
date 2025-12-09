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
    authserver_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders )

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

CollectIncludeDirectories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  authserver_INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders)

if( WIN32 )
  set(authserver_SOURCE_FILES
    ${authserver_SOURCE_FILES}
    ${CMAKE_SOURCE_DIR}/src/common/Debugging/WheatyExceptionReport.cpp
    ${CMAKE_SOURCE_DIR}/src/common/Debugging/WheatyExceptionReport.h
  )
  if ( MSVC )
    set(authserver_SOURCE_FILES
      ${authserver_SOURCE_FILES}
      authserver.rc
    )
  endif ()
endif()

add_executable(authserver
  ${authserver_SOURCE_FILES}
)

target_include_directories(authserver PRIVATE ${authserver_INCLUDES})

if( NOT WIN32 )
  set_target_properties(authserver PROPERTIES
    COMPILE_DEFINITIONS _TRINITY_REALM_CONFIG="${CONF_DIR}/authserver.conf"
  )
endif()

target_link_libraries(authserver
  shared
  ${MYSQL_LIBRARY}
  ${OPENSSL_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
)

if( WIN32 )
  if ( MSVC )
    add_custom_command(TARGET authserver
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/authserver.conf.dist ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
    )
  elseif ( MINGW )
    add_custom_command(TARGET authserver
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/authserver.conf.dist ${CMAKE_BINARY_DIR}/bin/
    )
  endif()
endif()

if( UNIX )
  install(TARGETS authserver DESTINATION bin)
  install(FILES  authserver.conf.dist DESTINATION ${CONF_DIR})
elseif( WIN32 )
  install(TARGETS authserver DESTINATION "${CMAKE_INSTALL_PREFIX}")
  install(FILES authserver.conf.dist DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()

# Generate precompiled header
if (USE_COREPCH)
  target_precompile_headers(authserver PUBLIC PrecompiledHeaders/authPCH.h)
endif()
