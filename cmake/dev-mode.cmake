enable_language(CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS YES)

include(CTest)

include(FetchContent)

FetchContent_Declare(
   ut
   GIT_REPOSITORY https://github.com/boost-ext/ut.git
   GIT_TAG master
   GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(ut)

# Done in developer mode only, so users won't be bothered by this :)
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/argz/*.hpp")
source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX headers FILES ${headers})

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
source_group(TREE "${PROJECT_SOURCE_DIR}/src" PREFIX sources FILES ${sources})

add_executable(argz_test ${sources} ${headers})

target_link_libraries(argz_test PRIVATE argz::argz ut)

add_test(NAME argz_test COMMAND argz_test)
