from conans import ConanFile

class FlowTestProject(ConanFile):
	name = "FlowTestProject"
	version = "bleed"
	description = """Contains the unit tests standalone eclipse project for Flow."""
	url = "https://github.com/CynaraKrewe/Flow"
	license = "MIT"
	author = "Mathias Spiessens"
	exports = "*"
	build_policy = "missing"
	requires = "FlowTest/bleed@cynara/testing"
	
	def source(self):
		self.run("git clone https://github.com/CynaraKrewe/Flow.git")

	def build(self):
		self.output.info("Nothing to build, this package provides a project.")

	def package(self):
		self.copy(".project", "", "Flow")
		self.copy(".cproject", "", "Flow")
		self.copy("main.cpp", "source", "Flow/source")
