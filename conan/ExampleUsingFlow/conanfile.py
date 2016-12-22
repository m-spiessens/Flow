from conans import ConanFile

class ExampleUsingFlow(ConanFile):
   requires = "FlowCore/bleed@cynara/testing", "FlowExtra/bleed@cynara/testing"
   
   def imports(self):
      self.copy("*.h",)
      self.copy("*.cpp")
