from conans import ConanFile, CMake

class GeeBee(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "Boost/1.60.0@lasote/stable", "sdl/2.0.4@chaosteil/stable"
    generators = "cmake"
