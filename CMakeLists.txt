cmake_minimum_required(VERSION 3.18)

include(cmake/prelude.cmake)

project(
    argz
    VERSION 0.2.0
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

add_library(argz_argz INTERFACE)
add_library(argz::argz ALIAS argz_argz)

if (MSVC)
   string(REGEX MATCH "\/cl(.exe)?$" matched_cl ${CMAKE_CXX_COMPILER})
   if (matched_cl)
      # for a C++ standards compliant preprocessor, not needed for clang-cl
      target_compile_options(argz_argz INTERFACE "/Zc:preprocessor" /GL /permissive- /Zc:lambda)
      target_link_options(argz_argz INTERFACE /LTCG /INCREMENTAL:NO)
   endif()
else()
   target_compile_options(argz_argz INTERFACE "-Wno-missing-braces")
endif()

set_property(TARGET argz_argz PROPERTY EXPORT_NAME argz)

target_compile_features(argz_argz INTERFACE cxx_std_20)
target_include_directories(
    argz_argz ${warning_guard}
    INTERFACE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(cmake/dev-mode.cmake)
endif()
