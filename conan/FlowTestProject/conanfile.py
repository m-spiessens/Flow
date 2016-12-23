from conans import ConanFile
from conans.tools import download, unzip
import os

class FlowTestProject(ConanFile):
	name = "FlowTestProject"
	version = "1.0"
	description = """Contains the unit tests standalone eclipse project for Flow."""
	url = "https://github.com/CynaraKrewe/Flow"
	license = "MIT"
	author = "Mathias Spiessens"
	exports = "*"
	build_policy = "missing"
	requires = "FlowTest/1.0@cynara/testing"
	
	def source(self):
		download("https://github.com/CynaraKrewe/Flow/archive/v1.0.zip", "Flow-1.0.zip")
		unzip("Flow-1.0.zip")
		os.unlink("Flow-1.0.zip")

	def build(self):
		self.output.info("Nothing to build, this package provides a project.")

	def package(self):
		self.copy(".project", "", "Flow")
		self.copy(".cproject", "", "Flow")
		self.copy("main.cpp", "source", "Flow/source")
