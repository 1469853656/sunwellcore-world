# Copyright (C) 
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB_RECURSE mmap_gen_sources *.cpp *.h)

add_executable(mmaps_generator ${mmap_gen_sources})

target_link_libraries(mmaps_generator
  common
  g3dlib
  Recast
  Detour
  ${BZIP2_LIBRARIES}
  ${ZLIB_LIBRARIES}
)

if(WITH_VS_FOLDERS)
set_target_properties(mmaps_generator PROPERTIES FOLDER Tools)
endif()

if( UNIX )
  install(TARGETS mmaps_generator DESTINATION bin)
elseif( WIN32 )
  install(TARGETS mmaps_generator DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()
