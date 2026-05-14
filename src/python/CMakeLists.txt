# Python bindings CMake configuration
find_package(Python COMPONENTS Interpreter Development REQUIRED)
# Find nanobind package
find_package(nanobind CONFIG REQUIRED)

# Create Python module target
nanobind_add_module(viscopuppy_python
    module.cpp
)


# Link with viscopuppy header-only library
target_link_libraries(viscopuppy_python PRIVATE viscopuppy)

# Include directories
target_include_directories(viscopuppy_python PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

# Set output name to match Python package
set_target_properties(viscopuppy_python PROPERTIES
    OUTPUT_NAME "viscopuppy"
    PREFIX ""
    SUFFIX "${CMAKE_SHARED_MODULE_SUFFIX}"
)

# Set module output directory to Python package location
set_target_properties(viscopuppy_python PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../python/viscopuppy
)

# Compile options
target_compile_options(viscopuppy_python PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Link options
target_link_options(viscopuppy_python PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wl,--as-needed>
    $<$<CXX_COMPILER_ID:Clang>:-Wl,--as-needed>
)