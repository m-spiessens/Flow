from conans import ConanFile, CMake

class Flow(ConanFile):
	name = "Flow"
	version = "2.0"
	description = """Flow is a pipes and filters implementation tailored for microcontrollers. 
		It provides 3 base concepts: component, port and connection."""
	url = "https://github.com/CynaraKrewe/Flow"
	license = "MIT"
	author = "Mathias Spiessens"
	build_policy = "missing"
	settings = { "arch": ["x86", "x86_64", "armv6", "armv7", "armv7hf"], "os": ["none", "Linux"], "build_type": ["Release", "Debug"], "compiler": ["gcc"] }
	generators = "cmake"
	exports_sources = "include/*", "source/*", "CMakeLists.txt"

	def build(self):
		cmake = CMake(self)
		if self.settings.os == "none":
			cmake.definitions["CMAKE_TRY_COMPILE_TARGET_TYPE"] = "STATIC_LIBRARY"
		if self.settings.arch == "armv6":
			cmake.definitions["CMAKE_CXX_FLAGS_INIT"] = "-march=armv6-m -mthumb -fno-exceptions"
		elif self.settings.arch == "armv7":
			cmake.definitions["CMAKE_CXX_FLAGS_INIT"] = "-march=armv7e-m -mthumb -fno-exceptions"
		elif self.settings.arch == "armv7hf":
			cmake.definitions["CMAKE_CXX_FLAGS_INIT"] = "-march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fno-exceptions"
		cmake.configure()
		cmake.build()

	def package(self):
		self.copy("*.h", "include/flow/", "include/flow/")
		self.copy("libFlow.a", "library/", "lib/")
		self.copy("components.cpp", "source/flow/", "source/flow/")
		self.copy("flow.cpp", "source/flow/", "source/flow/")
		self.copy("reactor.cpp", "source/flow/", "source/flow/")
		if self.settings.arch == "x86" or self.info.settings.arch == "x86_64":
			self.copy("platform_cpputest.cpp", "source/flow/", "source/flow/")
		elif self.settings.arch == "armv6" or self.settings.arch == "armv7" or self.settings.arch == "armv7hf":
			self.copy("platform_cortexm4.cpp", "source/flow/", "source/flow/")
		else:
			raise Exception("Invalid settings for this package.")

	def package_info(self):
		self.cpp_info.includedirs = ["include/"]
		self.cpp_info.libdirs = ["library/"]
		self.cpp_info.srcdirs = ["source/"]
		self.cpp_info.libs = ["Flow"]
