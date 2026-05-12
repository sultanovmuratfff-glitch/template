# Source directory CMake configuration

# Add math library subdirectory
add_subdirectory(mathlib)

# Add Python bindings subdirectory
add_subdirectory(python)

# Create main library target
add_library(viscopuppy INTERFACE)

# Link with math library
target_link_libraries(viscopuppy INTERFACE mathlib)

# Include directories
target_include_directories(viscopuppy INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Compile features
target_compile_features(viscopuppy INTERFACE cxx_std_23)

# Compile options
target_compile_options(viscopuppy INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic -Werror>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)

# Link options
target_link_options(viscopuppy INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:-Wl,--as-needed>
    $<$<CXX_COMPILER_ID:Clang>:-Wl,--as-needed>
)
