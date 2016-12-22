from conans import ConanFile

class FlowCore(ConanFile):
	name = "FlowCore"
	version = "bleed"
	description = """Flow is a pipes and filters implementation tailored for microcontrollers. 
		It provides 3 base concepts: component, port and connection."""
	url = "https://github.com/CynaraKrewe/Flow"
	license = "MIT"
	author = "Mathias Spiessens"
	exports = "*"
	build_policy = "missing"
	
	def source(self):
		self.run("git clone https://github.com/CynaraKrewe/Flow.git")

	def build(self):
		self.output.info("Nothing to build, this package provides sources.")

	def package(self):
		self.copy("queue.h", "flow/include", "Flow/flow/include")
		self.copy("flow.h", "flow/include", "Flow/flow/include")
