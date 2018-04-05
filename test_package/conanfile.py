from conans import ConanFile
import os
import shutil

class FlowTest(ConanFile):
	requires = 'CppUTest/3.8@spiessensm/stable'

	def build(self):
		self.run('eclipse --launcher.suppressErrors -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data temp -import project -cleanBuild "Flow/Release"')

	def imports(self):
		shutil.copytree('../../../include/flow_test', 'project/include/flow_test')
		shutil.copytree('../../../source/flow_test', 'project/source/flow_test')
		shutil.copy('../../../source/main.cpp', 'project')
		shutil.copy('../../../.project', 'project')
		shutil.copy('../../../.cproject', 'project')
		self.copy('*.h', dst='project')
		self.copy('*.cpp', dst='project')
		self.copy('*.a', dst='project')

	def test(self):
		self.run('./project/Release/Flow')
