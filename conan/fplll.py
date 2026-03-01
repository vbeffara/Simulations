from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.errors import ConanInvalidConfiguration
from conan.tools.files import copy, get, rmdir, rm
from conan.tools.gnu import Autotools, AutotoolsToolchain
from conan.tools.layout import basic_layout
from conan.tools.apple import fix_apple_shared_install_name


class CairoConan(ConanFile):
    name = "fplll"
    version = "5.5.0"
    license = "LGPL"
    url = "https://github.com/fplll/fplll"
    description = "Lattice algorithms using floating-point arithmetic"
    settings = "os", "compiler", "build_type", "arch"
    requires = "mpfr/4.2.0"
    package_type = "library"
    generators = "AutotoolsDeps"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {
        "shared": True,
        "fPIC": True,
    }

    def source(self):
        get(
            self,
            "https://github.com/fplll/fplll/releases/download/5.5.0/fplll-5.5.0.tar.gz",
            strip_root=True,
        )

    def generate(self):
        tc = AutotoolsToolchain(self)
        tc.generate()

    def build(self):
        autotools = Autotools(self)
        autotools.configure()
        autotools.make()

    def package(self):
        autotools = Autotools(self)
        autotools.install()
        fix_apple_shared_install_name(self)

    def package_info(self):
        self.cpp_info.libs = ["fplll"]


# required_conan_version = ">=1.54.0"


# class PackageConan(ConanFile):
#     homepage = "https://openpam.org/"
#     topics = ("pam", "pluggable-authentication-module", "authentication", "security")

#     def config_options(self):
#         if self.settings.os == "Windows":
#             del self.options.fPIC

#     def configure(self):
#         if self.options.shared:
#             self.options.rm_safe("fPIC")
#         self.settings.rm_safe("compiler.libcxx")
#         self.settings.rm_safe("compiler.cppstd")

#     def layout(self):
#         basic_layout(self, src_folder="src")

#     def build(self):
#         autotools = Autotools(self)
#         autotools.configure()
#         autotools.make()

#     def package(self):
#         autotools = Autotools(self)
#         autotools.install()
#         copy(
#             self,
#             "LICENSE",
#             self.source_folder,
#             os.path.join(self.package_folder, "licenses"),
#         )
#         rmdir(self, os.path.join(self.package_folder, "share"))
#         rm(self, "*.la", os.path.join(self.package_folder, "lib"))
