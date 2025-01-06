project('atlas', 'cpp', default_options: ['cpp_std=c++20'])


atlas = declare_dependency(include_directories: include_directories('include'))

test_sources = files('tests/main.cpp', 'tests/option.cpp',
  'tests/result.cpp', 'tests/impl.cpp', 'tests/string_view.cpp',
  'tests/string.cpp', 'tests/iter.cpp', 'tests/array.cpp',
  'tests/vec.cpp', 'tests/cons.cpp', 'tests/slice.cpp',
  'tests/smallvec.cpp', 'tests/arc.cpp', 'tests/box.cpp',
  'tests/cursor.cpp', 'tests/elf.cpp', 'tests/rbtree.cpp',
  'tests/map.cpp', 'tests/dot.cpp', 'tests/hashmap.cpp',
  'tests/pairing_heap.cpp', 'tests/bitmap.cpp', 'tests/hamt.cpp', 'tests/fmt.cpp', 'tests/list.cpp'

                    )

test('all_tests',
     executable(
       'tests',
       sources: test_sources,
       cpp_args: ['-g', '-fsanitize=address', '--coverage', '-DDEBUG_CHECKS=1'],
       link_args: ['-fsanitize=address', '--coverage'],
       dependencies: [atlas, dependency('doctest')],
       native: true)
 )


deps = [
  atlas,
  dependency('benchmark'),
  dependency('absl_flat_hash_map'),
]

#executable('bench', sources: ['benchmarks/main.cpp', 'tests/impl.cpp'],
#           dependencies: deps, include_directories: include_directories('frigg/include'))

