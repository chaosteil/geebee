from conans import ConanFile, CMake
import traceback

class GeeBee(ConanFile):
    name = "GeeBee"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    requires = "Boost/1.60.0@lasote/stable", "sdl/2.0.4@chaosteil/stable", "catch/1.3.0@TyRoXx/stable"
    generators = "cmake"
