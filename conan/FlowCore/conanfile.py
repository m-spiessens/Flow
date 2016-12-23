from conans import ConanFile

class FlowCore(ConanFile):
	name = "FlowCore"
	version = "1.0"
	description = """Flow is a pipes and filters implementation tailored for microcontrollers. 
		It provides 3 base concepts: component, port and connection."""
	url = "https://github.com/CynaraKrewe/Flow"
	license = "MIT"
	author = "Mathias Spiessens"
	exports = "*"
	build_policy = "missing"
	
	def source(self):
		download("https://github.com/CynaraKrewe/Flow/archive/Flow-1.0.zip", "Flow-1.0.zip")
		unzip("Flow-1.0.zip")
		os.unlink("Flow-1.0.zip")

	def build(self):
		self.output.info("Nothing to build, this package provides sources.")

	def package(self):
		self.copy("queue.h", "include/flow", "Flow/include/flow")
		self.copy("flow.h", "include/flow", "Flow/include/flow")
