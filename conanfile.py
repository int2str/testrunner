import os

from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy
from conan.tools.scm import Version


class TestRunnerConan(ConanFile):
    name = "testrunner"
    version = "0.1.0"
    description = "A simple C++ unit test framework"
    license = "GPL-3.0-or-later"
    package_type = "static-library"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    exports_sources = "CMakeLists.txt", "cmake/*", "include/*", "src/*", "LICENSE"

    def layout(self):
        cmake_layout(self)

    def validate(self):
        check_min_cppstd(self, "23")
        compiler_version = Version(self.settings.compiler.version)
        if self.settings.compiler == "gcc" and compiler_version < "14":
            raise ConanInvalidConfiguration("TestRunner requires GCC 14 or newer for C++23 <print> support")
        if self.settings.compiler == "clang" and compiler_version < "18":
            raise ConanInvalidConfiguration("TestRunner requires Clang 18 or newer for C++23 <print> support")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "LICENSE", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "testrunner")

        self.cpp_info.components["testrunner"].set_property(
            "cmake_target_name", "testrunner::testrunner"
        )
        self.cpp_info.components["testrunner"].libs = ["testrunner"]

        self.cpp_info.components["main"].set_property(
            "cmake_target_name", "testrunner::main"
        )
        self.cpp_info.components["main"].libs = ["testrunner_main"]
        self.cpp_info.components["main"].requires = ["testrunner"]
