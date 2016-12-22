from conans import ConanFile

class ExampleUsingFlow(ConanFile):
   requires = "FlowTestProject/bleed@cynara/testing"
   
   def imports(self):
      self.copy("*.h")
      self.copy("*.cpp")
      self.copy("*.a")
      self.copy(".project")
      self.copy(".cproject")
