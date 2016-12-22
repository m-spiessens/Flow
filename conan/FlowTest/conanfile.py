from conans import ConanFile

class FlowTest(ConanFile):
	name = "FlowTest"
	version = "bleed"
	description = """Contains the unit tests for Flow."""
	url = "https://github.com/CynaraKrewe/Flow"
	license = "MIT"
	author = "Mathias Spiessens"
	exports = "*"
	build_policy = "missing"
	requires = "FlowExtra/bleed@cynara/testing", "CppUTest/3.8@cynara/testing"
	
	def source(self):
		self.run("git clone https://github.com/CynaraKrewe/Flow.git")

	def build(self):
		self.output.info("Nothing to build, this package provides sources.")

	def package(self):
		self.copy("*.h", "include/flow_test", "Flow/include/flow_test")
		self.copy("*.cpp", "source/flow_test", "Flow/source/flow_test")
