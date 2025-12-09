# We require CMake >= 3.16
cmake_minimum_required(VERSION 3.16)

# add this options before PROJECT keyword
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(SunwellCore)

# Set RPATH-handing (CMake parameters)
set(CMAKE_SKIP_BUILD_RPATH 0)
set(CMAKE_BUILD_WITH_INSTALL_RPATH 0)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH 1)

# set macro-directory
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/macros")

# build in Release-mode by default if not explicitly set
if( NOT CMAKE_BUILD_TYPE )
  set(CMAKE_BUILD_TYPE "Release")
endif()

# set required standard to C++20
set(CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 20)

# set default buildoptions or load them from cache then apply
include(cmake/options.cmake)

include(CheckPlatform)
include(GroupSources)
include(Collect)

if (WIN32)
  # On windows the requirements are higher according to the wiki.
  set(BOOST_REQUIRED_VERSION 1.70)
else()
  set(BOOST_REQUIRED_VERSION 1.68)
endif()

set(TARGET_PACKAGES system filesystem program_options iostreams regex)

if (WIN32)
  set(TARGET_PACKAGES ${TARGET_PACKAGES} bzip2)
endif()

find_package(Boost ${BOOST_REQUIRED_VERSION} REQUIRED ${TARGET_PACKAGES})
find_package(OpenSSL "1.1.0" QUIET REQUIRED)
find_package(Threads QUIET REQUIRED)

# hidden config
if( NOT USE_MYSQL_SOURCES )
  find_package(MySQL QUIET REQUIRED)
endif()

if( UNIX )
  find_package(Readline QUIET)
  find_package(ZLIB QUIET)
  find_package(BZip2 QUIET)
endif()

# find git and verify version
if(NOT WITHOUT_GIT)
  find_package(Git "1.7" QUIET)
endif()

#everything is ok so far, prepare targets
add_compile_definitions("BOOST_ALL_NO_LIB")
include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})

# Find revision ID and hash of the sourcetree
include(cmake/genrev.cmake)

# print out the results before continuing
include(cmake/showoptions.cmake)

# add dependencies
add_subdirectory(dep)

# add core sources
add_subdirectory(src)
