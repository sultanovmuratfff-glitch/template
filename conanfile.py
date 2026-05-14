from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.build import check_min_cppstd
from conan.tools.scm import Version
import os


class ViscopuppyConan(ConanFile):
    name = "viscopuppy"
    version = "0.1.0"
    license = "MIT"
    author = "Your Name <your.email@example.com>"
    url = "https://github.com/yourusername/viscopuppy"
    description = "A high-performance Python library built from modern C++23 code"
    topics = ("python", "c++23", "nanobind", "high-performance")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_tests": [True, False],
        "with_examples": [True, False],
        "with_python": [True, False],
        "with_coverage": [True, False],
        "with_sanitizers": [True, False],
        "with_lto": [True, False],
    }
    default_options = {
        "shared": True,
        "fPIC": True,
        "with_tests": False,
        "with_examples": False,
        "with_python": False,
        "with_coverage": False,
        "with_sanitizers": False,
        "with_lto": True,
    }
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "tests/*", "python/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def requirements(self):
        # Test dependencies (only if building tests)
        if self.options.with_tests:
            self.requires("catch2/3.5.0")
        
        # Python development dependencies (only if building python bindings)
        if self.options.with_python:
            self.requires("nanobind/2.12.0")
            self.requires("pybind11/2.11.1", transitive_headers=True)

    def validate(self):
        # Check for C++23 support
        check_min_cppstd(self, 23)
        
        # Check compiler versions
        compiler = self.settings.compiler
        version = Version(str(self.settings.compiler.version))
        
        if compiler == "gcc":
            if version < "13":
                raise ConanInvalidConfiguration("viscopuppy requires GCC 13 or later for C++23 support")
        elif compiler == "clang":
            if version < "16":
                raise ConanInvalidConfiguration("viscopuppy requires Clang 16 or later for C++23 support")
        elif compiler == "msvc":
            if version < "193":
                raise ConanInvalidConfiguration("viscopuppy requires MSVC 2022 or later for C++23 support")
        else:
            self.output.warning(f"Compiler {compiler} may not fully support C++23 features")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        
        # CMake definitions
        tc.variables["BUILD_SHARED_LIBS"] = self.options.shared
        tc.variables["BUILD_TESTING"] = self.options.with_tests
        tc.variables["BUILD_EXAMPLES"] = self.options.with_examples
        tc.variables["ENABLE_COVERAGE"] = self.options.with_coverage
        tc.variables["ENABLE_SANITIZERS"] = self.options.with_sanitizers
        tc.variables["ENABLE_LTO"] = self.options.with_lto
        tc.variables["CMAKE_CXX_STANDARD"] = 23
        tc.variables["CMAKE_CXX_STANDARD_REQUIRED"] = True
        tc.variables["CMAKE_CXX_EXTENSIONS"] = False
        
        # Python configuration
        tc.variables["VISCOPUPPY_BUILD_PYTHON"] = self.options.with_python
        if self.options.with_python:
            tc.variables["PYTHON_EXECUTABLE"] = self.dependencies["pybind11"].cpp_info.bindirs[0]
        
        # Generator
        tc.generate()
        
        # Generate dependencies
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        
        if self.options.with_tests:
            cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "viscopuppy")
        self.cpp_info.set_property("cmake_target_name", "viscopuppy::viscopuppy")
        
        # Library information
        self.cpp_info.libs = ["viscopuppy"]
        self.cpp_info.includedirs = ["include"]
        
        # Python module
        if self.options.with_python:
            self.cpp_info.bindirs.append(os.path.join(self.package_folder, "python", "viscopuppy"))
        
        # Set system dependencies if needed
        if self.settings.os in ["Linux", "FreeBSD"]:
            self.cpp_info.system_libs = ["m", "pthread"]
        
        # Runtime requirements for Python
        if self.options.with_python:
            self.cpp_info.requires = ["nanobind::nanobind", "pybind11::pybind11"]

    # def package_id(self):
        # if self.options.shared:
        #     self.info.settings.compiler.version = self.settings.compiler.version