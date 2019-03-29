# Project definition
project('c-t9', 'c',
    version: '1.0.1',
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

    # We are using GNU extensions.
    '-DGNUC',
    '-DGNU_COMPILER',
])
add_project_arguments(compiler_args, language: 'c')

# Configuration
configuration = configuration_data()


math_dep = compiler.find_library('m', required : true)

# Dependencies list
dependencies = [
  math_dep,
]

# Version.
version = meson.project_version()
version_array = version.split('.')
configuration.set_quoted('CT9_VERSION', version)
configuration.set('CT9_VERSION_MAJOR', version_array[0])
configuration.set('CT9_VERSION_MINOR', version_array[1])
configuration.set('CT9_VERSION_PATCH', version_array[2])

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
