from conans import ConanFile, CMake
from conans.tools import download, unzip
import os
import shutil

class Flow(ConanFile):
	name = "Flow"
	version = "2.0"
	description = """Flow is a pipes and filters implementation tailored for microcontrollers. 
		It provides 3 base concepts: component, port and connection."""
	url = "https://github.com/CynaraKrewe/Flow"
	license = "MIT"
	author = "Mathias Spiessens"
	build_policy = "missing"
	settings = { "arch": ["x86", "x86_64", "armv7hf"], "os": ["Linux"], "build_type": ["Release", "Debug"], "compiler": ["gcc"] }
	generators = "cmake"
	exports_sources = "include/*", "source/*", "CMakeLists.txt"

	def build(self):
		cmake = CMake(self)
		cmake.verbose = True
		cmake.configure()
		cmake.build()

	def package(self):
		self.copy("components.h", "flow/include/", "include/")
		self.copy("flow.h", "flow/include/", "include/")
		self.copy("platform.h", "flow/include/", "include/")
		self.copy("pool.h", "flow/include/", "include/")
		self.copy("queue.h", "flow/include/", "include/")
		self.copy("reactor.h", "flow/include/", "include/")
		self.copy("utility.h", "flow/include/", "include/")
		self.copy("libFlow.a", "flow/library/", "lib/")
		if self.settings.arch == "x86" or self.info.settings.arch == "x86_64":
			self.copy("platform_cpputest.cpp", "flow/source/", "source/")
		elif self.settings.arch == "armv7hf":
			self.copy("platform_cortexm4.cpp", "flow/source/", "source/")
		else:
			raise Exception("Invalid settings for this package.")

	def package_info(self):
		self.cpp_info.includedirs = ["flow/include/"]
		self.cpp_info.libdirs = ["flow/library/"]
		self.cpp_info.srcdirs = ["flow/source/"]
		self.cpp_info.libs = ["Flow"]
