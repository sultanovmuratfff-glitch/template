# Math library CMake configuration

# Create math library target
add_library(mathlib INTERFACE)

# Include directories
target_include_directories(mathlib INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Compile features
target_compile_features(mathlib INTERFACE cxx_std_23)

# Compile options
target_compile_options(mathlib INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic -Werror>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)

# Link options
target_link_options(mathlib INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:-Wl,--as-needed>
    $<$<CXX_COMPILER_ID:Clang>:-Wl,--as-needed>
)

# Export for installation
install(TARGETS mathlib EXPORT viscopuppy-targets)