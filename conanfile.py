from conan import ConanFile
from conan.tools.cmake import cmake_layout


class LogScopeDependencies(ConanFile):
    name = "logscope"
    version = "0.1.0"
    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        # Match the fmt version required by ConanCenter's spdlog/1.17.0 recipe.
        self.requires("fmt/12.1.0")
        self.requires("spdlog/1.17.0")
        self.requires("nlohmann_json/3.12.0")

    def build_requirements(self):
        self.test_requires("gtest/1.18.0")

    def layout(self):
        cmake_layout(self)
