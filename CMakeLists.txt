cmake_minimum_required(VERSION 3.25)
project(viscopuppy VERSION 0.1.0 LANGUAGES CXX)

# Modern CMake policies
cmake_policy(SET CMP0077 NEW)  # option() honors normal variables
cmake_policy(SET CMP0135 NEW)  # CMAKE_CUDA_ARCHITECTURES validation

# Set C++ standard to 23 with required features
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable position independent code for shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Enable interprocedural optimization for release builds
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Build configuration options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTING "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(ENABLE_LTO "Enable link time optimization" ON)
option(VISCOPUPPY_USE_CONAN "Use Conan for dependency management" OFF)
option(VISCOPUPPY_BUILD_PYTHON "Build Python bindings" ON)

# Package manager configuration
if(CONAN_EXPORTED)
    message(STATUS "Building with Conan")
else()
    # Find or download Conan
    find_program(CONAN_COMMAND conan)
    if(NOT CONAN_COMMAND)
        message(WARNING "Conan not found. Please install Conan 2.0+")
    endif()
endif()

# Add subdirectories
add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Installation configuration
include(GNUInstallDirs)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT Development
)

install(
    TARGETS viscopuppy
    EXPORT viscopuppy-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT Runtime
)

if(VISCOPUPPY_BUILD_PYTHON)
    install(
        TARGETS viscopuppy_python
        EXPORT viscopuppy-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT Runtime
    )
endif()

# Export targets for find_package
install(
    EXPORT viscopuppy-targets
    FILE viscopuppy-config.cmake
    NAMESPACE viscopuppy::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/viscopuppy
    COMPONENT Development
)

# Package configuration
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/viscopuppy-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/viscopuppy-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/viscopuppy
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/viscopuppy-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/viscopuppy-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/viscopuppy-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/viscopuppy
    COMPONENT Development
)

# Code coverage
if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage)
        add_link_options(--coverage)
    endif()
endif()

# Sanitizers
if(ENABLE_SANITIZERS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(
            -fsanitize=address
            -fsanitize=undefined
            -fsanitize=leak
        )
        add_link_options(
            -fsanitize=address
            -fsanitize=undefined
            -fsanitize=leak
        )
    endif()
endif()

# Link time optimization
if(ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_ERROR)
    if(IPO_SUPPORTED)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    else()
        message(WARNING "LTO not supported: ${IPO_ERROR}")
    endif()
endif()

# Summary
message(STATUS "=========================================")
message(STATUS "viscopuppy Configuration Summary")
message(STATUS "=========================================")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Shared Libraries: ${BUILD_SHARED_LIBS}")
message(STATUS "Build Tests: ${BUILD_TESTING}")
message(STATUS "Build Examples: ${BUILD_EXAMPLES}")
message(STATUS "Enable Coverage: ${ENABLE_COVERAGE}")
message(STATUS "Enable Sanitizers: ${ENABLE_SANITIZERS}")
message(STATUS "Enable LTO: ${ENABLE_LTO}")
message(STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "=========================================")