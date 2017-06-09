from conans import ConanFile
from conans.tools import download, unzip
import os
import shutil

class FlowTest(ConanFile):
	name = "FlowTest"
	version = "1.2"
	description = """Contains the unit tests for Flow."""
	url = "https://github.com/CynaraKrewe/Flow"
	license = "MIT"
	author = "Mathias Spiessens"
	build_policy = "missing"
	requires = "FlowExtra/1.2@spiessensm/stable", "CppUTest/3.8@spiessensm/stable"
	
	def source(self):
		download("https://github.com/CynaraKrewe/Flow/archive/v1.2.zip", "Flow-1.2.zip")
		unzip("Flow-1.2.zip")
		shutil.move("Flow-1.2", "Flow")
		os.unlink("Flow-1.2.zip")

	def build(self):
		self.output.info("Nothing to build, this package provides sources.")

	def package(self):
		self.copy("*.h", "include/flow_test", "Flow/include/flow_test")
		self.copy("*.cpp", "source/flow_test", "Flow/source/flow_test")
