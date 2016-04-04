project(geebee)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Werror")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(src)
file(GLOB_RECURSE GEEBEE_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Exclude main file for library generation, only use it in the final
# executable. This lets us test everything without compiling twice.
set(GEEBEE_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
list(REMOVE_ITEM GEEBEE_SOURCE ${GEEBEE_MAIN})

include_directories(${CONAN_INCLUDE_DIRS})
add_library(geebeelib STATIC ${GEEBEE_SOURCE})
add_executable(geebee ${GEEBEE_MAIN})
target_link_libraries(geebee geebeelib ${CONAN_LIBS})

enable_testing()
add_subdirectory(tests)
