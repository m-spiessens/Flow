from conans import ConanFile
from conans.tools import download, unzip
import os
import shutil

class FlowExtra(ConanFile):
	name = "FlowExtra"
	version = "1.0"
	description = """Contains predefined components and utilities which could be useful when using Flow."""
	url = "https://github.com/CynaraKrewe/Flow"
	license = "MIT"
	author = "Mathias Spiessens"
	build_policy = "missing"
	requires = "FlowCore/1.0@spiessensm/stable"
	
	def source(self):
		download("https://github.com/CynaraKrewe/Flow/archive/v1.0.zip", "Flow-1.0.zip")
		unzip("Flow-1.0.zip")
		shutil.move("Flow-1.0", "Flow")
		os.unlink("Flow-1.0.zip")

	def build(self):
		self.output.info("Nothing to build, this package provides sources.")

	def package(self):
		self.copy("utility.h", "include/flow", "Flow/include/flow")
		self.copy("components.h", "include/flow", "Flow/include/flow")
		self.copy("components.cpp", "source/flow", "Flow/source/flow")
