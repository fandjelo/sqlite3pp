from conan import ConanFile
from conan.tools.build import can_run
from conan.tools.cmake import cmake_layout
from conan.tools.cmake import CMake

class Pkg(ConanFile):
    name = "database"
    version = "0.1.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"
    requires = [ "sqlite3/[>=3.8]" ]
    build_requires = [ "cmake/[>=3.18]" ]
    test_requires = [ "gtest/[>=1.12]" ]

    def package_info(self):
        self.cpp_info.libs = ["database"]

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if can_run(self):
            cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()
