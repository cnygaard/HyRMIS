from setuptools import setup, Extension
import platform

# Determine the appropriate OpenMP flags for the current platform
if platform.system() == "Darwin":  # macOS
    extra_compile_args = ['-Xpreprocessor', '-fopenmp', '-O3']
    extra_link_args = ['-lomp']
else:  # Linux and others
    extra_compile_args = ['-fopenmp', '-O3']
    extra_link_args = ['-fopenmp']

module = Extension('hyrmis',
                  sources=['hyrmis-fully-parallel.c'],
                  extra_compile_args=extra_compile_args,
                  extra_link_args=extra_link_args)

setup(name='hyrmis',
      version='1.0',
      description='Hybrid Radix-Merge-Insertion Sort',
      ext_modules=[module])
