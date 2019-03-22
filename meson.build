# Project definition
project('c-t9', 'c',
    version: '0.0.2',
    license: 'MIT',
    default_options: ['c_std=c11'],
meson_version: '>=0.46.0')

# Set compiler warning flags
compiler = meson.get_compiler('c')
compiler_args = compiler.get_supported_arguments([
    '-pedantic',
    '-Wall',
    '-Wextra',
    # Explicitly enable noisy warnings.
    '-Wmissing-declarations',
    '-Wmissing-prototypes',
    '-Wstrict-prototypes',
    '-Wbad-function-cast',
    '-Wsign-compare',
    '-Wnested-externs',
    '-Wshadow',
    '-Waggregate-return',
    '-Wcast-align',
    '-Wold-style-definition',
    '-Wdeclaration-after-statement',
    '-Wuninitialized',
    #'-Wshorten-64-to-32',
])
math_dep = compiler.find_library('m', required : true)
add_project_arguments(compiler_args, language: 'c')

# Configuration
configuration = configuration_data()

# Dependencies list
dependencies = [
  math_dep,
]

# Version
version = meson.project_version()

# Libraries
subdir('libraries')

# Includes
include_dir = include_directories('include')
subdir('include')

# Sources
subdir('src')

# Build executable
ct9 = executable(meson.project_name(), sources + libsources,
    dependencies: dependencies,
    include_directories: include_dir,
    install: false)
