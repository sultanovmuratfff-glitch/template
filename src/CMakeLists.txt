# Source directory CMake configuration

# Create header-only library target (all quadrature / SOE code is in headers)
add_library(viscopuppy INTERFACE)

# Include directories
target_include_directories(viscopuppy INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Compile features
target_compile_features(viscopuppy INTERFACE cxx_std_23)

# Compile options
target_compile_options(viscopuppy INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Link options
target_link_options(viscopuppy INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:-Wl,--as-needed>
    $<$<CXX_COMPILER_ID:Clang>:-Wl,--as-needed>
)

# Add Python bindings subdirectory (requires nanobind from conan)
if(VISCOPUPPY_BUILD_PYTHON)
    add_subdirectory(python)
endif()
