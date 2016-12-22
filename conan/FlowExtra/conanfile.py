from conans import ConanFile

class FlowExtra(ConanFile):
	name = "FlowExtra"
	version = "bleed"
	description = """Contains predefined components and utilities which could be useful when using Flow."""
	url = "https://github.com/CynaraKrewe/Flow"
	license = "MIT"
	author = "Mathias Spiessens"
	exports = "*"
	build_policy = "always"
	requires = "FlowCore/bleed@cynara/testing"
	
	def source(self):
		self.run("git clone https://github.com/CynaraKrewe/Flow.git")

	def build(self):
		self.output.info("Nothing to build, this package provides sources.")

	def package(self):
		self.copy("utility.h", "include/flow", "Flow/flow/include")
		self.copy("components.h", "include/flow", "Flow/flow/include")
		self.copy("components.cpp", "source/flow", "Flow/flow/source")
		self.copy("debug.h", "include/flow", "Flow/flow/include")
		self.copy("debug.cpp", "source/flow", "Flow/flow/source")
