# Tests CMake configuration

# Find Catch2 package
find_package(Catch2 3 REQUIRED)
include(Catch)

# Create test executable
add_executable(unit_tests test_mathlib.cpp)

# Link with math library and Catch2
target_link_libraries(unit_tests PRIVATE mathlib Catch2::Catch2WithMain)

# Include directories
target_include_directories(unit_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Compile options
target_compile_options(unit_tests PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)
