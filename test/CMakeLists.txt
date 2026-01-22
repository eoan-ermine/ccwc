Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sample/test.txt ${CMAKE_CURRENT_BINARY_DIR}/test.txt COPYONLY)

include(Catch)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)

add_executable(sample sample/main.cpp)
target_link_libraries(sample PRIVATE Catch2::Catch2WithMain libcount libccwc)
add_test(sample_test sample)

catch_discover_tests(sample)
