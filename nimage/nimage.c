/*
   NImage 
   --- A Ruby Extension Library for displaying Images on X11
       (C) Copyright 2000 by Masahiro TANAKA
*/

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "ruby.h"

static VALUE cNImage;

struct NIMG {
  char  *image;
  int    width;
  int    height;
  Window window;
};

#define MAX_COLOR 256

static Display *display;
static GC       gc=NULL;
static XColor   color[MAX_COLOR];

static int ncolor = 32;
static int color_allocated = 0;

static void color_alloc_gray()
{
  int i;
  Colormap cmap;

  cmap = DefaultColormap(display, 0);
  for (i=0; i<ncolor; i++) {
    color[i].red   = 
    color[i].green = 
    color[i].blue  = (i*65535)/ncolor;
    if ( !XAllocColor( display, cmap, &color[i] ) )
      rb_raise(rb_eRuntimeError, "ERROR: cannot allocate Color\n" );
  }
  color_allocated = 1;
}


static void color_alloc(int n, char *r, char *g, char *b)
{
  int i;
  Colormap cmap;

  cmap = DefaultColormap(display, 0);
  for (i=0; i<n; i++) {
    color[i].red   = r[i]*256;
    color[i].green = g[i]*256;
    color[i].blue  = b[i]*256;
    if ( !XAllocColor( display, cmap, &color[i] ) )
      rb_raise(rb_eRuntimeError, "ERROR: cannot allocate Color\n" );
  }
  ncolor = n;
  color_allocated = 1;
}


static void color_free()
{
  int i;
  Colormap cmap;

  if (color_allocated) {
    cmap = DefaultColormap(display, 0);
    for (i=0; i<ncolor; i++)
      XFreeColors(display, cmap, &color[i].pixel, 1, 0);
    color_allocated = 0;
  }
}

static VALUE nimg_colormap(VALUE self, VALUE val_r, VALUE val_g, VALUE val_b)
{
  char *r, *g, *b;
  int nr, ng, nb, n;

  r = rb_str2cstr(val_r, &nr);
  g = rb_str2cstr(val_g, &ng);
  b = rb_str2cstr(val_b, &nb);

  n = (nr<ng) ? nr:ng;
  n = (n <nb) ? n :nb;
  if (n > MAX_COLOR)
    rb_raise(rb_eArgError, "exceed maximum # of colors");

  color_free();
  color_alloc(n,r,g,b);
  XFlush(display);

  return Qnil;
}

static VALUE nimg_ncolors(VALUE self)
{
  return INT2NUM(ncolor);
}

static XImage *create_ximage(struct NIMG *nimg)
{
  XImage *image;
  Visual *visual;
  int malloc_len;
  int depth;

  depth = DefaultDepth(display, DefaultScreen(display));
  visual = CopyFromParent;

  image = XCreateImage( display, visual, depth,
			ZPixmap, 0, 0, nimg->width, nimg->height,
			32, 0 );

  malloc_len = image->bytes_per_line * image->height;
  image->data = (unsigned char*) malloc(malloc_len);
  memset(image->data, 0, malloc_len);

  return image;
}


static void nimg_put_image(struct NIMG *nimg)
{
  XImage *ximage;
  int i, j, k;

  if (!color_allocated)
    color_alloc_gray();

  ximage = create_ximage(nimg);

  k=0;
  for (j=nimg->height-1; j>=0; j--)
    for (i=0; i<nimg->width; i++)
      XPutPixel(ximage, i,j, color[nimg->image[k++]].pixel);

  XPutImage( display, nimg->window, gc,
	     ximage, 0, 0,
	     0,0, nimg->width, nimg->height );

  XFlush(display);
  XDestroyImage(ximage);
}


static void init_display(void)
{
  display = XOpenDisplay( NULL );
  if ( display==NULL )
    rb_raise(rb_eRuntimeError, "ERROR: cannot open display\n");
}


static void init_wm(struct NIMG *nimg)
{
  XTextProperty  text;
  XSizeHints     size;

  /* for window managers */
  text.value    = (unsigned char*)"NImage";
  text.encoding = 31;
  text.format   = 8;
  text.nitems   = strlen( (char*)text.value );

  size.flags = PPosition | PMinSize | PMaxSize;
  /*size.flags  = PPosition | PSize;*/
  size.width =
  size.min_width =
  size.max_width = nimg->width;
  size.height =
  size.min_height = 
  size.max_height = nimg->height;
  size.x = 0;
  size.y = 0;

  XSetWMProperties( display, nimg->window,
		    &text, &text, NULL, 0, &size, NULL, NULL );
}


static void show_window(Window window)
{
  XEvent event;
  /* show window */
  XMapWindow( display, window );
  do XNextEvent( display, &event );
  while ( event.type != Expose );
}


static void nimg_init_window(struct NIMG *nimg)
{
  int screen, depth;
  Window parent, window;
  Visual *visual;
  XSetWindowAttributes attr;
  unsigned long bgcolor;
  unsigned long mask = CWBackingStore | CWEventMask;
  unsigned long event_mask =
    ExposureMask
    | ButtonPressMask
    | ButtonReleaseMask
    | EnterWindowMask
    | LeaveWindowMask
    | KeyPressMask
    | KeyReleaseMask
    | PointerMotionMask
    | StructureNotifyMask ;

  screen = DefaultScreen( display );
  parent = RootWindow( display, screen );
  depth  = DefaultDepth( display, screen );
  visual = DefaultVisual( display, screen );
  bgcolor = BlackPixel( display, screen );
  
  attr.backing_store = /*Always*/ WhenMapped;
  attr.event_mask = event_mask;

  nimg->window = 
    XCreateWindow( display, parent,
		   0, 0, nimg->width, nimg->height,
		   1, depth, InputOutput, visual,
		   mask, &attr);
  if (gc==NULL)
    gc = XCreateGC( display, nimg->window, 0, 0 );

  XSetWindowBackground( display, nimg->window, bgcolor );

  init_wm( nimg );
  show_window( nimg->window );
}


/* free contents */
static void
 nimg_free(struct NIMG* nimg)
{
  free( nimg->image );
  free( nimg );
}


static VALUE
 nimg_new( VALUE self, VALUE val_image, VALUE val_wid, VALUE val_hei )
{
  struct NIMG *nimg;
  char *data;
  int   i, len;

  data = rb_str2cstr(val_image,&len);

  nimg = ALLOC( struct NIMG );
  nimg->image = ALLOC_N( char, len );
  MEMCPY( nimg->image, data, char, len );

  nimg->width  = NUM2INT(val_wid);
  nimg->height = NUM2INT(val_hei);
 
  if (len != nimg->width * nimg->height)
    rb_raise(rb_eArgError, "Image size mismatch");

  nimg_init_window(nimg);
  nimg_put_image(nimg);

  return Data_Wrap_Struct(cNImage, NULL, nimg_free, nimg);
}


static VALUE
 nimg_update( VALUE self, VALUE val_image )
{
  struct NIMG *nimg;
  char *data;
  int   i, len;

  Data_Get_Struct( self, struct NIMG, nimg );
  data = rb_str2cstr(val_image,&len);

  if (len != nimg->width * nimg->height)
    rb_raise(rb_eArgError, "Image size mismatch");

  MEMCPY( nimg->image, data, char, len );

  nimg_put_image(nimg);
  return Qnil;
}


static VALUE
 nimg_close( VALUE self )
{
  struct NIMG *nimg;

  Data_Get_Struct( self, struct NIMG, nimg );
  XDestroyWindow( display, nimg->window );
  nimg->window = 0;
  XFlush( display );
  return self;
}


void
  Init_nimage_c()
{
  /* open display, etc. */
  init_display();

  /* define NImage class */
  cNImage = rb_define_class( "NImage", rb_cObject );

  /* class methods */
  rb_define_singleton_method( cNImage, "new", nimg_new, 3 );
  rb_define_singleton_method( cNImage, "colormap", nimg_colormap, 3 );
  rb_define_singleton_method( cNImage, "ncolors", nimg_ncolors, 0 );

  /* method */
  rb_define_method( cNImage, "update", nimg_update, 1 );
  rb_define_method( cNImage, "close", nimg_close, 0 );
}
