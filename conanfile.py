#
# MIT License
#
# Copyright (c) 2023 Filipp Andjelo
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
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

