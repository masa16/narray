require 'mkmf'
have_header("X11/Xlib.h")
have_header("X11/Xutil.h")
have_library("X11", "XOpenDisplay")
create_makefile("nimage_c")
