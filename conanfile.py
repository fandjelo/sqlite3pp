from conan import ConanFile
from conan.tools.build import can_run
from conan.tools.cmake import CMake, cmake_layout

class sqlite3ppRecipe(ConanFile):

    name = "sqlite3pp"
    version = "0.5.0"
    package_type = "library"

    # Optional information
    license = "MIT"
    author = "Filipp Andjelo <filipp.andjelo@gmail.com>"
    url = "https://github.com/fandjelo/sqlite3pp"
    description = "C++ wrapper around sqlite3"
    topics = "sqlite3"

    # Options
    options = {
        "shared": [True, False], 
        "fPIC": [True, False],
        "with_tests": [True, False],
    }
    default_options = {
        "shared": True, 
        "fPIC": True,
        "with_tests": True
    }

    # Other settings
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"
    exports_sources = "CMakeLists.txt", "src/*"

    def requirements(self):
        #self.tool_requires("cmake/[>=3.18]")
        self.requires("sqlite3/[>=3.8]")
        if self.options.with_tests:
            self.test_requires("gtest/[>=1.12]")

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def package_info(self):
        self.cpp_info.libs = ["sqlite3pp"]

    def build(self):
        variables = { "SQLITE3PP_WITH_TESTS" : self.options.with_tests }
        cmake = CMake(self)
        cmake.configure(variables)
        cmake.build()
        if self.options.with_tests and can_run(self):
            cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def layout(self):
        cmake_layout(self)

