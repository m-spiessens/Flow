from conans import ConanFile

class Flow(ConanFile):
   requires = "Flow/1.5@spiessensm/stable", "CppUTest/3.8@spiessensm/stable"
   
   def imports(self):
      self.copy("*.h")
      self.copy("*.cpp")
      self.copy("*.a")
