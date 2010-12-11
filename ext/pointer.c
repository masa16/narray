/*
  pointer.c
  Numerical Array Extention for Ruby
    (C) Copyright 1999-2007 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#include <math.h>
#include <ruby.h>
//#include <version.h>
#include "narray.h"
//#include "narray_local.h"
#include "bytedata.h"

#include "template.h"
#include "template_data.h"


static void
pointer_new_loop( na_iterator_t *const itr, VALUE info )
{
    size_t  i;
    char   *p1, *p2;
    ssize_t s1, s2;
    ssize_t *idx1, *idx2;
    void    *x;
    INIT_COUNTER( itr, i );
    INIT_PTR( itr, 0, p1, s1, idx1 );
    INIT_PTR( itr, 1, p2, s2, idx2 );
    if (idx1||idx2) {
	for (; i--; ) {
	    LOAD_PTR_STEP( p1, s1, idx1, void, x );
	    STORE_DATA_STEP( p2, s2, idx2, void*, x );
	}
    } else {
	for (; i--; ) {
	    *(void **)p2 = p1;
	    p1+=s1;
	    p2+=s2;
	}
    }
}

VALUE
 num_pointer_new( VALUE a1 )
{
    ndfunc_t *func;
    VALUE v;
    func = ndfunc_alloc( pointer_new_loop, HAS_LOOP,
			 1, 1, Qnil, cPointer );
    v = ndfunc_execute( func, 1, a1 );
    ndfunc_free(func);
    return v;
}


typedef struct {
    void *beg;
    ssize_t step;
} pointer_index_t;


static void
pointer_index_loop( na_iterator_t *const itr, VALUE opt )
{
    size_t  i;
    char   *p1, *p2;
    ssize_t s1, s2;
    ssize_t *idx1, *idx2;
    int      e2;
    void    *x;
    u_int64_t y;
    void    *beg;
    int64_t  step;
    pointer_index_t *g;

    INIT_COUNTER( itr, i );
    INIT_PTR(     itr, 0, p1, s1, idx1 );
    INIT_PTR_ELM( itr, 1, p2, s2, idx2, e2 );
    Data_Get_Struct(opt, pointer_index_t, g);
    beg = g->beg;
    step = g->step;
    if (idx1||idx2) {
	for (; i--; ) {
	    LOAD_DATA_STEP( p1, s1, idx1, void*, x );
	    y = (x-beg)/step;
	    STORE_INT_STEP( p2, s2, idx2, e2, u_int64_t, y );
	}
    } else {
	for (; i--; ) {
	    x = *(void**)p1;
	    p1+=s1;
	    y = (x-beg)/step;
	    STORE_INT( p2, e2, y );
	    p2+=s2;
	}
    }
}

VALUE
num_pointer_index( VALUE ptr, void *beg, ssize_t step, VALUE idxclass )
{
    ndfunc_t *func;
    VALUE v, opt;
    pointer_index_t *g;

    g = ALLOC(pointer_index_t);
    g->beg = beg;
    g->step = step;
    opt = Data_Wrap_Struct(rb_cData,0,0,g);
    func = ndfunc_alloc( pointer_index_loop, HAS_LOOP,
			 1, 1, Qnil, idxclass );
    v = ndfunc_execute( func, 2, ptr, opt );
    ndfunc_free(func);
    return v;
}



void
Init_pointer()
{
    rb_define_const(cPointer, ELEMENT_BIT_SIZE,  INT2FIX(sizeof(void*)*8));
    rb_define_const(cPointer, ELEMENT_BYTE_SIZE, INT2FIX(sizeof(void*)));
    rb_define_const(cPointer, CONTIGUOUS_STRIDE, INT2FIX(sizeof(void*)));
}
