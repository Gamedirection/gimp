libappgui_sources = [
  'gimpapp.c',
  'gimpuiconfigurer.c',
  'gui-message.c',
  'gui-unique.c',
  'gui-vtable.c',
  'gui.c',
  'icon-themes.c',
  'modifiers.c',
  'session.c',
  'splash.c',
  'themes.c',
]

# Workaround for generated header included in other directories.
configInclude = include_directories('.')


libappgui = static_library('appgui',
  libappgui_sources,
  include_directories: [ rootInclude, rootAppInclude, ],
  c_args: '-DG_LOG_DOMAIN="Gimp-GUI"',
  dependencies: [
    cairo, gegl, gdk_pixbuf, gexiv2, gio_specific, gtk3
  ],
  install: false,
)
