from conans import ConanFile

class ExampleUsingFlow(ConanFile):
   requires = "Flow/1.5@spiessensm/stable"
   
   def imports(self):
      self.copy("*.h")
      self.copy("*.cpp")
      self.copy("*.a")
      self.copy(".project")
      self.copy(".cproject")
