from conans import ConanFile, CMake, tools


class YamlcppConan(ConanFile):
    name = "yaml-cpp"
    version = "0.6.1"
    license = "MIT"
    url = "https://github.com/jbeder/yaml-cpp"
    description = "yaml-cpp is a YAML parser and emitter in C++ matching the YAML 1.2 spec"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = "shared=True"
    generators = "cmake"

    def source(self):
        self.run("git clone https://github.com/jbeder/yaml-cpp.git")
        self.run("cd yaml-cpp && git checkout yaml-cpp-0.6.1")

        tools.replace_in_file("yaml-cpp/CMakeLists.txt", "project(YAML_CPP)", '''project(YAML_CPP)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(KEEP_RPATHS)''')

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="yaml-cpp")
        cmake.build()

    def package(self):
        self.copy("*.h", dst="include", src="yaml-cpp/include")
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so*", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["yaml-cpp"]
