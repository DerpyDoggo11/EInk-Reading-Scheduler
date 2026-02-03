project(
  'template-kindle-gtk',
  'cpp',
  version: 'v0.0.1',
  default_options: ['cpp_std=c++17', 'optimization=2'],
  meson_version: '>=1.1'
)

gtk_dep = dependency('gtk+-2.0')
sqlite_dep = dependency('sqlite3')

sources = files(
  './src/main.cpp',
  './src/icons/book_icon.h',
  './src/icons/quit_icon.h',
  './src/icons/streak_icon.h',
)

include_dirs = include_directories('./include/')

executable(
  'template_kindle_gtk',
  sources,
  include_directories: include_dirs,
  dependencies: [
    gtk_dep,
    sqlite_dep,
  ],
  cpp_args: ['-O2', '-static-libstdc++'],
  link_args: ['-static-libstdc++']
)
