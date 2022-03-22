cmake_minimum_required(VERSION 3.0)
project(argz)

# This flag allows visual studio projects to be organized into folders in the solution explorer.
#    It also forces CMAKE default targets (eg ALL_BUILD, INSTALL, ZERO_CHECK, etc) to be placed in a folder
#    of the solution called "CMakePredefinedTargets".
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
	add_definitions(/bigobj)
	# C++ exceptions
	add_definitions(/EHsc)
endif()

file(GLOB_RECURSE srcs include/argz/*.hpp)

# Organize files into directories
foreach(FILE ${srcs})
   # Get the directory of the source file
   get_filename_component(PARENT_DIR "${FILE}" PATH)

   # Remove common directory prefix to make the group
   string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

if(MSVC)
   # Make sure we are using windows slashes
   string(REPLACE "/" "\\" GROUP "${GROUP}")
endif()

   # Group into "Source Files" and "Header Files"
   if ("${FILE}" MATCHES ".*\\.cpp")
      set(GROUP "Source Files${GROUP}")
   elseif("${FILE}" MATCHES ".*\\.h")
      set(GROUP "Header Files${GROUP}")
   endif()

   source_group("${GROUP}" FILES "${FILE}")
endforeach()

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

set(CPM_LOCAL_PACKAGE_PATHS "" CACHE FILEPATH "Path to CMake file (.cmake) setting CPM_dep_SOURCE variables")

if(NOT ${CPM_LOCAL_PACKAGE_PATHS} STREQUAL "")
   include(${CPM_LOCAL_PACKAGE_PATHS})
endif()

include(cmake/CPM.cmake)

if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
   # This flag allows visual studio projects to be organized into folders 
	# in the solution explorer.
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	add_subdirectory(src)
endif()