from conans import ConanFile, CMake
import os
import shutil

class FlowTest(ConanFile):
      requires = "CppUTest/3.8@spiessensm/stable", "Flow/2.0@spiessensm/testing"
      settings = "os", "compiler", "build_type", "arch"
      generators = "cmake"

      def imports(self):
            self.copy("*.cpp", "flow/source/", "flow/source/")

      def build(self):
            cmake = CMake(self)
            cmake.verbose = True
            cmake.configure()
            cmake.build()

      def test(self):
            self.run('./bin/FlowTest')
