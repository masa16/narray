/*
  na_fftw.c
  Numerical Array Extention for Ruby
    (C) Copyright 1999-2002 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#include <ruby.h>
#include "narray.h"
#include <fftw.h>

VALUE rb_mFFTW;

#ifdef FFTW_ENABLE_FLOAT
#define NA_FFTW_TYPE NA_SCOMPLEX
#else
#define NA_FFTW_TYPE NA_DCOMPLEX
#endif


static VALUE
 na_fftw(VALUE m, VALUE val, VALUE vdir)
{
  struct NARRAY *a1, *a2;
  int i, dir, *shape;
  fftwnd_plan p;
  volatile VALUE v1, v2;

  dir = NUM2INT(vdir);
  if ( dir != 1 && dir != -1 )
    rb_raise(rb_eArgError, "direction should be 1 or -1");

  v1 = na_cast_unless_narray(val,NA_FFTW_TYPE);
  GetNArray(v1,a1);
  v2 = na_make_object( NA_FFTW_TYPE, a1->rank, a1->shape, CLASS_OF(v1) );
  GetNArray(v2,a2);
  na_copy_nary( a2, a1 );

  shape = ALLOCA_N(int, a2->rank);
  for (i=0; i<a2->rank; i++)
    shape[i] = a2->shape[a2->rank-1-i];

  p = fftwnd_create_plan( a2->rank, shape, dir,
			  FFTW_ESTIMATE | FFTW_IN_PLACE );
  fftwnd_one( p, (fftw_complex*)a2->ptr, NULL );
  fftwnd_destroy_plan(p);

  return v2;
}

void
 Init_fftw()
{
  rb_mFFTW = rb_define_module("FFTW");
  rb_define_module_function(rb_mFFTW, "fftw", na_fftw, 2);
}
