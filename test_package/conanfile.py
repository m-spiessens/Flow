from conans import ConanFile, CMake

class FlowTest(ConanFile):
      requires = "CppUTest/3.8@spiessensm/stable", "Flow/2.0@spiessensm/testing"
      settings = "os", "compiler", "build_type", "arch"
      default_options = {"Flow:coverage": True}
      generators = "cmake"

      def imports(self):
            self.copy("*.cpp", "source/flow/", "source/flow/")

      def build(self):
            cmake = CMake(self)
            cmake.configure()
            cmake.build()

      def test(self):
            self.run('./bin/FlowTest')
