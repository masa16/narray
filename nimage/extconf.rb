require 'mkmf'

# configure options:
#  --with-x11-dir=path
#  --with-x11-include=path
#  --with-x11-lib=path
dir_config("x11")

#have_header("X11/Xlib.h")
#have_header("X11/Xutil.h")
have_library("X11", "XOpenDisplay")
create_makefile("nimage_c")
