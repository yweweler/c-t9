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

# This checks are inspired by the GNOME project.
# Check if the project build from an git repo and if the git binary is actually available.
git_bin = find_program('git', required: false, native: true)
if run_command(
  ['test', '-d', join_paths(meson.source_root(), '.git')]).returncode() == 0 and git_bin.found()
  # git repo and git is available
  git_version_str = run_command([git_bin.path(), 'describe', '--all', '--long']).stdout().strip()
  git_version = '"' + git_version_str + '"'
else
  git_version = 'Unknown, shouldn\'t happen'
endif

configuration.set('CT9_GIT_DESCRIPTION', git_version)


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
