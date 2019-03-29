# Generate configuration header
configure_file(
    input: 'config.h.in',
    output: 'config.h',
    configuration: configuration,
    install_dir: '/'.join([get_option('includedir'), 't9'])
)

# Install headers
includes = files([
  'corpus.h',
  'errno.h',
  'io.h',
  'math.h',
  'model.h',
  'node.h',
  'path.h',
  'timer.h',
  'tree.h',
])
install_headers(includes, subdir: 't9')
