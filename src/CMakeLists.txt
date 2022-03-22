CPMAddPackage(
   NAME ut
   GIT_REPOSITORY https://github.com/boost-ext/ut.git
   GIT_TAG master
)

project(tester)
add_executable(${PROJECT_NAME} Main.cpp ${srcs})
target_link_libraries(${PROJECT_NAME} PRIVATE argz ut)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

if (MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror -Wshadow -Wextra -Wpedantic)
endif()