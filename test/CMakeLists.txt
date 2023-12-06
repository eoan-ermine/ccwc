Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sample/test.txt ${CMAKE_CURRENT_BINARY_DIR}/test.txt COPYONLY)

add_executable(sample sample/main.cpp)
target_link_libraries(sample PRIVATE Catch2::Catch2WithMain)
