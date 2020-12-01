from conans import ConanFile, CMake

class FlowTest(ConanFile):
      requires = "CppUTest/3.8@spiessensm/stable", "Flow/2.0@spiessensm/testing"
      settings = "os", "compiler", "build_type", "arch"
      generators = "cmake"

      def imports(self):
            self.copy("*.h")
            self.copy("*.cpp")

      def build(self):
            cmake = CMake(self)
            cmake.configure()
            if(self.settings.build_type == "Release"):
                  cmake.build(target="FlowTest")
            elif(self.settings.build_type == "Debug"):
                  cmake.build(target="FlowCoverage")

      def test(self):
            if(self.settings.build_type == "Release"):
                  self.run('./bin/FlowTest')
            elif(self.settings.build_type == "Debug"):
                  self.run('./bin/FlowCoverage')
