from conans import ConanFile, AutoToolsBuildEnvironment, tools


class CairoConan(ConanFile):
    name = "fplll"
    version = "5.4.1"
    license = "LGPL"
    url = "https://github.com/fplll/fplll"
    description = "Lattice algorithms using floating-point arithmetic"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = "shared=True"
    generators = "cmake"
    requires = "mpfr/4.1.0"

    def source(self):
        tools.get(
            "https://github.com/fplll/fplll/releases/download/5.4.1/fplll-5.4.1.tar.gz"
        )

    def build(self):
        autotools = AutoToolsBuildEnvironment(self)
        autotools.configure(configure_dir=self.source_folder + "/fplll-5.4.1")
        autotools.make()
        autotools.install()

    def package(self):
        self.copy("*", src="package")

    def package_info(self):
        self.cpp_info.libs = ["fplll"]
