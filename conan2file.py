import os
import re
from conan import ConanFile
from conan.tools.files import save, load
from conan.tools.gnu import AutotoolsToolchain, AutotoolsDeps
from conan.tools.microsoft import unix_path, VCVars, is_msvc
from conan.errors import ConanInvalidConfiguration
from conan.errors import ConanException
from conan.tools.cmake import CMakeDeps, CMakeToolchain, CMake, cmake_layout


class Boost_Json_ArchiveConan(ConanFile):
    name = "boost_json_archive"
    author = "<Ceber> <contact@cedev.pro>"
    url = "https://github.com/Ceber/boost_json_archive"
    description = "C++ Boost::boost_json_archive 2nd party - Json for boost::serialization."

    settings = "os", "compiler", "build_type", "arch"

    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "build_tests": [True, False],
        "code_coverage": [True, False],
        "build_doc": [True, False],
        "build_tools": [True, False],
    }

    default_options = {
        "shared": True,
        "fPIC": True,
        "build_tests": False,
        "code_coverage": False,
        "build_doc": False,
        "build_tools": False,
    }

    requires = ("boost/[>=1.75.0]")

    exports_sources = [
        "src/*",
        "include/*",
        "cmake/*",
        "unit-tests/*",
        "doc/*",
        "README.md",
        "LICENSE",
        "CMakeLists.txt",
        "boost_json_archiveConfig.cmake.in",
    ]

    def set_version(self):
        content = load(self, os.path.join(self.recipe_folder, "CMakeLists.txt"))

        version_major = (
            re.search(r"[^\)]+VERSION_MAJOR (\d+)[^\)]*\)", content).group(1).strip()
        )
        version_minor = (
            re.search(r"[^\)]+VERSION_MINOR (\d+)[^\)]*\)", content).group(1).strip()
        )
        version_patch = (
            re.search(r"[^\)]+VERSION_PATCH (\d+)[^\)]*\)", content).group(1).strip()
        )

        self.version = "{}.{}.{}".format(version_major, version_minor, version_patch)


    def configure(self):
        # Boost options
        self.options["boost"].shared = True
        self.options["boost"].without_python = False
        # self.options["boost"].python_version = 3.10
        # self.options["boost"].python_executable = "python3"
        self.options["boost"].without_chrono = False
        self.options["boost"].without_serialization = False
        self.options["boost"].without_json = False
        self.options["boost"].without_type_erasure = False
        self.options["boost"].without_fiber = True
        self.options["boost"].without_wave = True
        self.options["boost"].without_graph = True
        self.options["boost"].without_graph_parallel = True
        self.options["boost"].without_coroutine = True
        self.options["boost"].without_log = True
        self.options["boost"].without_contract = True
        self.options["boost"].without_test = True
        self.options["boost"].without_type_erasure = True
        self.options["boost"].without_locale = True
        self.options["boost"].without_program_options = False
        # If any issue to 'conan create' related to ZLIB:
        # Copy/Paste and Rename zlib libraries as needed!


    # Defines the build directory among other things.
    def layout(self):
        cmake_layout(self)
        # Override the paths set by cmake_layout and add the architecture to the build folder
        if self.settings.get_safe("arch"):
            self.folders.build = "build/{}-{}-{}-conan-build".format(
                self.settings.os, self.settings.build_type, self.settings.arch
            )
            self.folders.generators = os.path.join(self.folders.build, "conan")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        toolchain = CMakeToolchain(self)
        toolchain.variables["CONAN_BUILD"] = True
        toolchain.variables["BUILD_TESTS"] = (
            self.options.build_tests == True or self.options.code_coverage == True
        )
        toolchain.variables["BUILD_DOC"] = self.options.build_doc == True
        toolchain.variables["WITH_COVERAGE"] = self.options.code_coverage == True

        # Override BASE_INSTALL_DIR. It is used by CPack.
        toolchain.variables["BASE_INSTALL_DIR"] = self.package_folder

        toolchain.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        # These are default values and doesn't need to be adjusted
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.builddirs = ["lib/CMake"]
        self.cpp_info.bindirs = ["bin"]
        self.cpp_info.requires = [
            "boost::system",
            "boost::serialization",
            "boost::json",
        ]
        self.cpp_info.libs = [
            "boost_json_archive",
        ]
