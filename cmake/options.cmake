# Copyright (C) 
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

option(SERVERS           "Build worldserver"                                           1)
option(AUTHSERVER        "Build also authserver"                                       0)
option(SCRIPTS           "Build core with scripts included"                            1)
option(TOOLS             "Build map/vmap/mmap extraction/assembler tools"              0)
option(USE_SCRIPTPCH     "Use precompiled headers when compiling scripts"              1)
option(USE_COREPCH       "Use precompiled headers when compiling servers"              1)
option(WITH_WARNINGS     "Show all warnings during compile"                            0)
option(WITH_COREDEBUG    "Include additional debug-code in core"                       0)
option(ADDRESS_SANITIZER "Use Address Sanitizer"                                       0)
option(THREAD_SANITIZER  "Use Thread Sanitizer"                                        0)
option(WITH_PROFILER     "Use EasyProfiler"                                            0)
option(WITHOUT_GIT       "Disable the GIT testing routines"                            0)
option(BOOST_STATIC      "Use Boost static libs"                                       0)
set(WITH_SOURCE_TREE     "hierarchical" CACHE STRING "Build the source tree for IDE's." )
set_property(CACHE WITH_SOURCE_TREE PROPERTY STRINGS no flat hierarchical hierarchical-folders)
option(WITH_VS_FOLDERS   "Enables Organizational Folders in Visual Studio"             0)
option(DISABLE_DRESSNPC_CORESOUNDS "Disable server side 'missing sounds' workaround"   0)

# interpret options

if(WITH_VS_FOLDERS)
    set_property(GLOBAL PROPERTY USE_FOLDERS On)
endif()

# turn off PCH totally if enabled (hidden setting, mainly for devs)
if( NOPCH )
  set(USE_COREPCH 0)
  set(USE_SCRIPTPCH 0)
endif()

# Be sure to search only static lib
if(BOOST_STATIC)
  set(Boost_USE_STATIC_LIBS ON)
endif()
