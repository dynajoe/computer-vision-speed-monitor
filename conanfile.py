from conans import ConanFile, CMake, tools

class ComputerVision(ConanFile):
    name = "computer-vision"
    version = "0.1"
    license = "MIT"
    author = "Joe"
    url = ""
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    exports_sources = "*"
    requires =  ("Catch2/2.7.0@catchorg/stable",
                 "spdlog/1.3.1@bincrafters/stable",
                 "opencv/4.0.1@conan/stable")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("**/*", dst="include", src="include")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib*", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["computer-vision"]
