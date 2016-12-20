from conans import ConanFile

class Moo(ConanFile):
   requires = "FlowCore/bleed@cynara/testing", "FlowExtra/bleed@cynara/testing"
   
   def imports(self):
      self.copy("*.h",)
      self.copy("*.cpp*")
