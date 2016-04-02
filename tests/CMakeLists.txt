cmake_minimum_required(VERSION 3.5)

file(GLOB_RECURSE TEST_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(geebee_test ${TEST_SOURCE})
target_link_libraries(geebee_test geebeelib ${CONAN_LIBS})
add_test(NAME geebee_tests
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         COMMAND ${CMAKE_BINARY_DIR}/bin/geebee_test --force-colour)
