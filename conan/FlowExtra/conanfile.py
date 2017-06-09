from conans import ConanFile
from conans.tools import download, unzip
import os
import shutil

class FlowExtra(ConanFile):
	name = "FlowExtra"
	version = "1.2"
	description = """Contains predefined components and utilities which could be useful when using Flow."""
	url = "https://github.com/CynaraKrewe/Flow"
	license = "MIT"
	author = "Mathias Spiessens"
	build_policy = "missing"
	requires = "FlowCore/1.2@spiessensm/stable"
	
	def source(self):
		download("https://github.com/CynaraKrewe/Flow/archive/v1.2.zip", "Flow-1.2.zip")
		unzip("Flow-1.2.zip")
		shutil.move("Flow-1.2", "Flow")
		os.unlink("Flow-1.2.zip")

	def build(self):
		self.output.info("Nothing to build, this package provides sources.")

	def package(self):
		self.copy("components.h", "include/flow", "Flow/include/flow")
		self.copy("components.cpp", "source/flow", "Flow/source/flow")
