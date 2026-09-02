project('canzan', 'c')

gtkdep = dependency('gtk4')

gnome = import('gnome')
resources = gnome.compile_resources('canzan', 'data/canzan.gresource.xml', source_dir: 'data')

sourcefiles = files('src/main.c')

executable('canzan', ['src/main.c', resources], dependencies: gtkdep, install: false)

