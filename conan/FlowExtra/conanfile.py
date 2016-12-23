from conans import ConanFile

class FlowExtra(ConanFile):
	name = "FlowExtra"
	version = "1.0"
	description = """Contains predefined components and utilities which could be useful when using Flow."""
	url = "https://github.com/CynaraKrewe/Flow"
	license = "MIT"
	author = "Mathias Spiessens"
	exports = "*"
	build_policy = "missing"
	requires = "FlowCore/1.0@cynara/testing"
	
	def source(self):
		self.run("git clone https://github.com/CynaraKrewe/Flow.git")

	def build(self):
		self.output.info("Nothing to build, this package provides sources.")

	def package(self):
		self.copy("utility.h", "include/flow", "Flow/include/flow")
		self.copy("components.h", "include/flow", "Flow/include/flow")
		self.copy("components.cpp", "source/flow", "Flow/source/flow")
