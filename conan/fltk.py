from conans import ConanFile, CMake, tools


class FltkConan(ConanFile):
    name = "fltk"
    version = "1.3.7"
    license = "LGPL"
    url = "http://www.fltk.org/"
    description = "FLTK is a cross-platform C++ GUI toolkit"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    requires = "libjpeg/9d", "libpng/1.6.37"

    def source(self):
        self.run(
            "git clone --branch release-1.3.7 --depth 1 http://github.com/fltk/fltk.git"
        )

    def build(self):
        cmake = CMake(self)
        if self.settings.os == "Linux":
            cmake.definitions["CMAKE_C_FLAGS"] = "-fPIC"
            cmake.definitions["CMAKE_CXX_FLAGS"] = "-fPIC"
        if self.settings.os == "Macos":
            cmake.definitions["CMAKE_C_COMPILER"] = "/usr/bin/clang"
            cmake.definitions["CMAKE_CXX_COMPILER"] = "/usr/bin/clang++"
        cmake.configure(source_folder="fltk")
        cmake.build()

    def package(self):
        self.copy("FL/*", dst="include", src="fltk")
        self.copy("*abi-version.h", dst="include/FL", keep_path=False)
        self.copy("*fltk.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so*", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)
        self.copy("*fluid", dst="bin", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["fltk", "fltk_gl"]
        if self.settings.os == "Macos":
            self.cpp_info.sharedlinkflags = [
                "-framework",
                "OpenGL",
                "-framework",
                "Cocoa",
            ]
            self.cpp_info.exelinkflags = ["-framework", "OpenGL", "-framework", "Cocoa"]
        if self.settings.os == "Linux":
            self.cpp_info.libs += [
                "GLU",
                "GL",
                "X11",
                "Xext",
                "pthread",
                "Xinerama",
                "Xfixes",
                "Xcursor",
                "Xft",
                "Xrender",
                "m",
                "fontconfig",
                "dl",
            ]
