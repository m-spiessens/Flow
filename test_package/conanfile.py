from conans import ConanFile, CMake
import os

class FlowTest(ConanFile):
      requires = "CppUTest/3.8@spiessensm/stable", "Flow/2.0@spiessensm/testing"
      settings = "os", "compiler", "build_type", "arch"
      options = { "coverage": [True, False] }
      default_options = { "coverage": False }
      generators = "cmake"

      def imports(self):
            self.copy("*.cpp", "source/flow/", "source/flow/")

      def build(self):
            cmake = CMake(self)
            cmake.configure()
            cmake.build(target="FlowTest")

            if self.options.coverage:
                  cmake.test(target="FlowCoverage")
                  test = os.path.join(self.build_folder, "bin", "FlowCoverage")
                  self.run(test)

      def test(self):
            test = os.path.join(self.build_folder, "bin", "FlowTest")
            self.run(test)
