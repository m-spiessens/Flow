from conans import ConanFile

class ExampleUsingFlow(ConanFile):
   requires = "FlowTestProject/1.0@spiessensm/stable"
   
   def imports(self):
      self.copy("*.h")
      self.copy("*.cpp")
      self.copy("*.a")
      self.copy(".project")
      self.copy(".cproject")
