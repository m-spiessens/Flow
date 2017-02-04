from conans import ConanFile
from conans.tools import download, unzip
import os
import shutil

class FlowCore(ConanFile):
	name = "FlowCore"
	version = "1.1"
	description = """Flow is a pipes and filters implementation tailored for microcontrollers. 
		It provides 3 base concepts: component, port and connection."""
	url = "https://github.com/CynaraKrewe/Flow"
	license = "MIT"
	author = "Mathias Spiessens"
	build_policy = "missing"
	
	def source(self):
		download("https://github.com/CynaraKrewe/Flow/archive/v1.1.zip", "Flow-1.1.zip")
		unzip("Flow-1.1.zip")
		shutil.move("Flow-1.1", "Flow")
		os.unlink("Flow-1.1.zip")

	def build(self):
		self.output.info("Nothing to build, this package provides sources.")

	def package(self):
		self.copy("queue.h", "include/flow", "Flow/include/flow")
		self.copy("utility.h", "include/flow", "Flow/include/flow")
		self.copy("flow.h", "include/flow", "Flow/include/flow")
		self.copy("flow.cpp", "source/flow", "Flow/source/flow")
