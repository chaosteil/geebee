project(geebee)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(src)
file(GLOB_RECURSE GEEBEE_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

include_directories(${CONAN_INCLUDE_DIRS})
add_executable(geebee ${GEEBEE_SOURCE})
target_link_libraries(geebee ${CONAN_LIBS})
