/*
  bit.c
  Numerical Array Extention for Ruby
    (C) Copyright 1999-2007 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#include <ruby.h>
//#include <version.h>
#include "narray.h"
//#include "narray_local.h"
#include "bytedata.h"

#include "template.h"
#include "template_int.h"


VALUE num_bit_debug_print( VALUE ary );

static VALUE num_cast_array_to_bit( VALUE ary );


static VALUE
num_bit_cast_numeric( VALUE val )
{
    narray_t *na;
    VALUE v;
    BIT_DIGIT *ptr, b=2;
    size_t dig_ofs;
    int    bit_ofs;

    if (FIXNUM_P(val)) {
	b = FIX2INT(val);
    } else if (val==Qtrue) {
	b = 1;
    } else if (val==Qfalse) {
	b = 0;
    }
    if (b!=0 && b!=1) {
	rb_raise(rb_eArgError, "bit can be cast from 0 or 1 or true or false");
    }

    v = rb_narray_new( cBit, 0, NULL );
    GetNArray(v,na);
    dig_ofs = na->offset / NB;
    bit_ofs = na->offset % NB;
    ptr = (BIT_DIGIT*)na_data_pointer_for_write( v ) + dig_ofs;
    *ptr = *ptr & ~(1u<<bit_ofs) | (b<<bit_ofs);
    return v;
}


static VALUE
num_bit_s__cast(VALUE bit, VALUE obj)
{
    if (CLASS_OF(obj)==cBit) {
	return obj;
    } else if (TYPE(obj)==T_ARRAY) {
	//shape = rb_funcall(cNArray,rb_intern("array_shape"),1,obj);
	return num_cast_array_to_bit(obj);
    } else if ( TYPE(obj)==T_FLOAT || FIXNUM_P(obj) || TYPE(obj)==T_BIGNUM ) {
	//printf("cast float\n");
	return num_bit_cast_numeric( obj );
    }
    return Qnil;
}


static VALUE
 num_bit__cast_to( VALUE value, VALUE type )
{
    return Qnil;
}


void bit_print_loop( char *ptr, size_t pos, VALUE opt )
{
    int x;
    LOAD_BIT(ptr,pos,x);
    //printf("%d", (((BIT_DIGIT*)ptr)[pos/NB] >> (pos%NB)) & 1u);
    printf("%d", x);
}

VALUE
num_bit_debug_print( VALUE ary )
{
    ndfunc_debug_print( ary, bit_print_loop, Qnil );
    return Qnil;
}



static VALUE
format_bit( VALUE fmt, int x )
{
    if (NIL_P(fmt)) {
	char s[4];
	int n;
	n = sprintf(s,"%1d",x);
	return rb_str_new(s,n);
    }
    return rb_funcall( fmt, '%', 1, INT2FIX(x) );
}

static VALUE
 bit_inspect_element( char *ptr, size_t pos, VALUE fmt )
{
    int x;
    LOAD_BIT(ptr,pos,x);
    return format_bit( fmt, x );
}
 VALUE
 num_bit_inspect( VALUE ary )
{
    VALUE str = na_info_str( ary );
    ndfunc_execute_inspect( ary, str, bit_inspect_element, Qnil );
    return str;
}


static void
bit_format_loop( na_iterator_t *const itr, VALUE fmt )
{
    size_t  i;
    BIT_DIGIT *a1, x=0;
    size_t     p1;
    char      *p2;
    ssize_t    s1, s2;
    ssize_t   *idx1, *idx2;
    VALUE y;

    INIT_COUNTER( itr, i );
    INIT_PTR_BIT( itr, 0, a1, p1, s1, idx1 );
    INIT_PTR( itr, 1, p2, s2, idx2 );
    for (; i--; ) {
	LOAD_BIT_STEP( a1, p1, s1, idx1, x );
	y = format_bit( fmt, x );
	STORE_DATA_STEP( p2, s2, idx2, VALUE, y );
    }
}
static VALUE
 num_bit_format( int argc, VALUE *argv, VALUE self )
{
     ndfunc_t *func;
     VALUE v, fmt=Qnil;

     rb_scan_args(argc, argv, "01", &fmt);
     func = ndfunc_alloc( bit_format_loop, HAS_LOOP,
			  1, 1, Qnil, cRObject );
     v = ndfunc_execute( func, 2, self, fmt );
     ndfunc_free(func);
     return v;
}
static VALUE
num_bit_format_to_a( int argc, VALUE *argv, VALUE self )
{
     ndfunc_t *func;
     VALUE v, fmt=Qnil;

     rb_scan_args(argc, argv, "01", &fmt);
     func = ndfunc_alloc( bit_format_loop, HAS_LOOP,
			  1, 1, Qnil, rb_cArray );
     v = ndfunc_execute_to_rarray( func, self, fmt );
     ndfunc_free(func);
     return v;
}




static void
 bit_fill_loop( na_iterator_t *const itr, VALUE x )
{
    size_t  n;
    size_t  p3;
    ssize_t s3;
    size_t *idx3;
    int     len;
    BIT_DIGIT *a3;
    BIT_DIGIT  y;

    if (x==INT2FIX(0) || x==Qfalse) {
	y = 0;
    } else
    if (x==INT2FIX(1) || x==Qtrue) {
	y = ~(BIT_DIGIT)0;
    } else {
	rb_raise(rb_eArgError, "invalid value for Bit");
    }

    INIT_COUNTER( itr, n );
    INIT_PTR_BIT( itr, 0, a3, p3, s3, idx3 );
    if ( s3!=1 || idx3 ) {
	y = y&1;
	for (; n--; ) {
	    STORE_BIT_STEP( a3, p3, s3, idx3, y );
	}
    } else {
	if (p3>0 || n<NB) {
	    len = NB - p3;
	    if (n<len) len=n;
	    *a3 = y & (SLB(len)<<p3) | *a3 & ~(SLB(len)<<p3);
	    a3++;
	    n -= len;
	}
	for (; n>=NB; n-=NB) {
	    *(a3++) = y;
	}
	if (n>0) {
	    *a3 = y & SLB(n) | *a3 & BALL<<n;
	}
    }
}

static VALUE
 num_bit_fill( VALUE self, VALUE val )
 {
     ndfunc_t *func;
     VALUE v;
     func = ndfunc_alloc( bit_fill_loop, HAS_LOOP, 1, 0, cBit );
     ndfunc_execute( func, 2, self, val );
     ndfunc_free(func);
     return self;
}


void
bit_cast_to_robj( na_iterator_t *const itr, VALUE info )
{
    size_t     i;
    ssize_t    s1, s2;
    size_t    *idx1, *idx2;
    BIT_DIGIT *a1;
    size_t     p1;
    char      *p2;
    BIT_DIGIT  x=0;
    VALUE      y;

    INIT_COUNTER( itr, i );
    INIT_PTR_BIT( itr, 0, a1, p1, s1, idx1 );
    INIT_PTR    ( itr, 1,     p2, s2, idx2 );
    for (; i--; ) {
	LOAD_BIT_STEP( a1, p1, s1, idx1, x );
	y = INT2FIX(x);
	STORE_DATA_STEP( p2, s2, idx2, VALUE, y );
    }
}


static VALUE
num_bit_cast_to_rarray( VALUE self )
{
    VALUE v;
    ndfunc_t *func;
    
    func = ndfunc_alloc( bit_cast_to_robj, HAS_LOOP, 1, 1, cBit, rb_cArray );
    v = ndfunc_execute_to_rarray( func, self, Qnil );
    ndfunc_free( func );
    return v;
}


static void
store_array_to_bit_loop( na_iterator_t *const itr, VALUE arg )
{
    size_t  i, n;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    char   *p1;
    size_t  p2;
    BIT_DIGIT *a2;
    VALUE     x;
    BIT_DIGIT y;

    INIT_COUNTER( itr, n );
    INIT_PTR    ( itr, 0,     p1, s1, idx1 );
    INIT_PTR_BIT( itr, 1, a2, p2, s2, idx2 );

    for (i=0; i<n; i++) {
	LOAD_DATA_STEP( p1, s1, idx1, VALUE, x );
	if (TYPE(x) != T_ARRAY) {
	    // NIL if empty
	    y = 2;
	    if (FIXNUM_P(x)) {
		y = FIX2INT(x);
	    } else if (x==Qtrue) {
		y = 1;
	    } else if (x==Qfalse) {
		y = 0;
	    }
	    if (y!=0 && y!=1) {
		rb_raise(rb_eArgError, "bit can be cast from 0 or 1 or true or false");
	    }
	    STORE_BIT_STEP( a2, p2, s2, idx2, y );
	}
    }
}


static VALUE
num_bit_store_array( VALUE nary, VALUE rary )
{
    VALUE v;
    ndfunc_t *func;

    func = ndfunc_alloc( store_array_to_bit_loop, HAS_LOOP,
			 2, 0, Qnil, rb_cArray );
    num_bit_fill( nary, INT2FIX(0) );
    ndloop_execute_store_array( func, rary, nary );
    ndfunc_free(func);
    return nary;
}

static VALUE
num_cast_array_to_bit( VALUE rary )
{
    int nd;
    size_t *shape;
    VALUE tp, nary;
    ndfunc_t *func;

    shape = na_mdarray_investigate(rary, &nd, &tp);
    nary = rb_narray_new(cBit, nd, shape);
    xfree(shape);
    num_bit_store_array(nary, rary);
    return nary;
}

static VALUE
num_bit_extract( VALUE self )
{
    BIT_DIGIT *ptr, val;
    size_t pos;
    narray_t *na;
    GetNArray(self,na);

    if (na->ndim==0) {
        ptr = (BIT_DIGIT*)na_data_pointer_for_read(self);
	pos = na->offset;
	val = ( (*((ptr)+(pos)/NB)) >> ((pos)%NB) ) & 1u;
	return INT2FIX(val);
    }
    return self;
}


#define UNARY_BIT_FUNC( opname, operation )			\
static void							\
 bit_##opname##_loop( na_iterator_t *const itr, VALUE info )	\
{								\
    size_t  n;							\
    size_t  p1, p3;						\
    ssize_t s1, s3;						\
    size_t *idx1, *idx3;					\
    int     o1, l1, r1, len;					\
    BIT_DIGIT *a1, *a3;						\
    BIT_DIGIT  x;						\
								\
    INIT_COUNTER( itr, n );					\
    INIT_PTR_BIT( itr, 0, a1, p1, s1, idx1 );			\
    INIT_PTR_BIT( itr, 1, a3, p3, s3, idx3 );			\
    if ( s1!=1 || s3!=1 || idx1 || idx3 ) {			\
	for (; n--; ) {						\
	    LOAD_BIT_STEP( a1, p1, s1, idx1, x );		\
	    {operation;};					\
	    STORE_BIT_STEP( a3, p3, s3, idx3, x );		\
	}							\
    } else {							\
	o1 =  p1 % NB;						\
	o1 -= p3;						\
	l1 =  NB+o1;						\
	r1 =  NB-o1;						\
	if (p3>0 || n<NB) {					\
	    len = NB - p3;					\
	    if (n<len) len=n;					\
	    if (o1>=0) x = *a1>>o1;				\
	    else       x = *a1<<-o1;				\
	    if (p1+len>NB)  x |= *(a1+1)<<r1;			\
	    a1++;						\
	    {operation;};					\
	    *a3 = x & (SLB(len)<<p3) | *a3 & ~(SLB(len)<<p3);	\
	    a3++;						\
	    n -= len;						\
	}							\
	if (o1==0) {						\
	    for (; n>=NB; n-=NB) {				\
		x = *(a1++);					\
		{operation;};					\
		*(a3++) = x;					\
	    }							\
	} else {						\
	    for (; n>=NB; n-=NB) {				\
		x = *a1>>o1;					\
		if (o1<0)  x |= *(a1-1)>>l1;			\
		if (o1>0)  x |= *(a1+1)<<r1;			\
		a1++;						\
		{operation;};					\
		*(a3++) = x;					\
	    }							\
	}							\
	if (n>0) {						\
	    x = *a1>>o1;					\
	    if (o1<0)  x |= *(a1-1)>>l1;			\
	    {operation;};					\
	    *a3 = x & SLB(n) | *a3 & BALL<<n;			\
	}							\
    }								\
}								\
								\
static VALUE							\
 num_bit_##opname( VALUE self )					\
 {								\
     ndfunc_t *func;						\
     VALUE v;							\
     func = ndfunc_alloc( bit_##opname##_loop, HAS_LOOP,	\
			     1, 1, cBit, cBit );		\
     v = ndfunc_execute( func, 1, self );			\
     ndfunc_free(func);						\
     return v;							\
}

UNARY_BIT_FUNC( copy, x=x )

VALUE
num_bit_store( VALUE dst, VALUE src )
{
    ndfunc_t *func;
    func = ndfunc_alloc( bit_copy_loop, HAS_LOOP,
			 2, 0, INT2FIX(1), Qnil );
    ndfunc_execute( func, 2, src, dst );
    ndfunc_free(func);
    return src;
}

VALUE na_aref(int argc, VALUE *argv, VALUE self);

/* method: []=(idx1,idx2,...,idxN,val) */
static VALUE
num_bit_aset(int argc, VALUE *argv, VALUE self)
{
    VALUE a;
    argc--;

    if (argc==0)
	num_bit_store( self, argv[argc] );
    else {
	a = na_aref( argc, argv, self );
	num_bit_store( a, argv[argc] );
    }
    return argv[argc];
}


//-- BIT AND --

#define BINARY_BIT_FUNC( opname, operation )			\
static void							\
 bit_##opname( na_iterator_t *const itr, VALUE info )		\
{								\
    size_t  n;							\
    size_t  p1, p2, p3;						\
    ssize_t s1, s2, s3;						\
    size_t *idx1, *idx2, *idx3;					\
    int     o1, o2, l1, l2, r1, r2, len;			\
    BIT_DIGIT *a1, *a2, *a3;					\
    BIT_DIGIT  x, y;						\
								\
    INIT_COUNTER( itr, n );					\
    INIT_PTR_BIT( itr, 0, a1, p1, s1, idx1 );			\
    INIT_PTR_BIT( itr, 1, a2, p2, s2, idx2 );			\
    INIT_PTR_BIT( itr, 2, a3, p3, s3, idx3 );			\
    if ( s1!=1 || s2!=1 || s3!=1 || idx1 || idx2 || idx3 ) {	\
	for (; n--; ) {						\
	    LOAD_BIT_STEP( a1, p1, s1, idx1, x );		\
	    LOAD_BIT_STEP( a2, p2, s2, idx2, y );		\
	    {operation;};					\
	    STORE_BIT_STEP( a3, p3, s3, idx3, x );		\
	}							\
    } else {							\
	o1 =  p1 % NB;						\
	o1 -= p3;						\
	o2 =  p2 % NB;						\
	o2 -= p3;						\
	l1 =  NB+o1;						\
	r1 =  NB-o1;						\
	l2 =  NB+o2;						\
	r2 =  NB-o2;						\
	if (p3>0 || n<NB) {					\
	    len = NB - p3;					\
	    if (n<len) len=n;					\
	    if (o1>=0) x = *a1>>o1;				\
	    else       x = *a1<<-o1;				\
	    if (p1+len>NB)  x |= *(a1+1)<<r1;			\
	    a1++;						\
	    if (o2>=0) y = *a2>>o2;				\
	    else       y = *a2<<-o2;				\
	    if (p2+len>NB)  y |= *(a2+1)<<r2;			\
	    a2++;						\
	    {operation;};					\
	    *a3 = x & (SLB(len)<<p3) | *a3 & ~(SLB(len)<<p3);	\
	    a3++;						\
	    n -= len;						\
	}							\
	if (o1==0 && o2==0) {					\
	    for (; n>=NB; n-=NB) {				\
		x = *(a1++);					\
		y = *(a2++);					\
		{operation;};					\
		*(a3++) = x;					\
	    }							\
	} else {						\
	    for (; n>=NB; n-=NB) {				\
		x = *a1>>o1;					\
		if (o1<0)  x |= *(a1-1)>>l1;			\
		if (o1>0)  x |= *(a1+1)<<r1;			\
		a1++;						\
		y = *a2>>o2;					\
		if (o2<0)  y |= *(a2-1)>>l2;			\
		if (o2>0)  y |= *(a2+1)<<r2;			\
		a2++;						\
		{operation;};					\
		*(a3++) = x;					\
	    }							\
	}							\
	if (n>0) {						\
	    x = *a1>>o1;					\
	    if (o1<0)  x |= *(a1-1)>>l1;			\
	    y = *a2>>o2;					\
	    if (o2<0)  y |= *(a2-1)>>l2;			\
	    {operation;};					\
	    *a3 = x & SLB(n) | *a3 & BALL<<n;			\
	}							\
    }								\
}								\
								\
static VALUE							\
num_bit_s_##opname( VALUE mod, VALUE a1, VALUE a2 )		\
 {								\
     ndfunc_t *func;						\
     VALUE v;							\
     func = ndfunc_alloc( bit_##opname, HAS_LOOP,		\
			     2, 1, cBit, cBit, cBit );		\
     v = ndfunc_execute( func, 2, a1, a2 );			\
     ndfunc_free(func);						\
     return v;							\
}

BINARY_BIT_FUNC( and, x&=y )
BINARY_BIT_FUNC( or,  x&=y )
BINARY_BIT_FUNC( xor, x^=y )

UNARY_BIT_FUNC( not, x=~x )



#define ACCUM_UNARY_FUNC( type, opname, condition )		\
static void							\
 bit_##opname##_##type##_loop( na_iterator_t *const itr, VALUE info )	\
{								\
    size_t  i;							\
    BIT_DIGIT *a1;						\
    size_t  p1;							\
    char   *p2;							\
    ssize_t s1, s2;						\
    size_t *idx1, *idx2;					\
    BIT_DIGIT x=0;						\
    type    y;							\
    type   *p;							\
								\
    INIT_COUNTER( itr, i );					\
    INIT_PTR_BIT( itr, 0, a1, p1, s1, idx1 );			\
    INIT_PTR( itr, 1, p2, s2, idx2 );				\
    if (idx1||idx2) {						\
	for (; i--; ) {						\
	    LOAD_BIT_STEP( a1, p1, s1, idx1, x );		\
	    if (idx2) {						\
		p = (type*)(p2 + *idx2);			\
		idx2++;						\
	    } else {						\
		p = (type*)(p2);				\
		p2 += s2;					\
	    }							\
	    if (condition) {					\
		y = *p;						\
		y++;						\
		*p = y;						\
	    }							\
	}							\
    } else {							\
	for (; i--; ) {						\
	    LOAD_BIT( a1, p1, x );				\
	    p1+=s1;						\
	    if (condition) {					\
		y = *(type*)p2;					\
		y++;						\
		*(type*)p2 = y;					\
	    }							\
	    p2+=s2;						\
	}							\
    }								\
}

ACCUM_UNARY_FUNC(int32_t,count_true, (x!=0))
ACCUM_UNARY_FUNC(int64_t,count_true, (x!=0))
ACCUM_UNARY_FUNC(int32_t,count_false,(x==0))
ACCUM_UNARY_FUNC(int64_t,count_false,(x==0))

static VALUE
 num_bit_count_true( int argc, VALUE *argv, VALUE self )
{
    VALUE v, mark;
    ndfunc_t *func;

    mark = na_mark_dimension( argc, argv, self );
    if (RNARRAY_SIZE(self) > 4294967295ul)
	func = ndfunc_alloc( bit_count_true_int64_t_loop, HAS_LOOP,
				1, 1, cBit, cInt64 );
    else
        func = ndfunc_alloc( bit_count_true_int32_t_loop, HAS_LOOP,
				1, 1, cBit, cInt32 );
    func->args[1].init = INT2FIX(0);
    v = ndfunc_execute_reduce( func, mark, 1, self );
    v = rb_funcall(v,rb_intern("extract"),0);
    ndfunc_free(func);
    return v;
}

static VALUE
 num_bit_count_false( int argc, VALUE *argv, VALUE self )
{
    VALUE v, mark;
    ndfunc_t *func;

    mark = na_mark_dimension( argc, argv, self );
    if (RNARRAY_SIZE(self) > 4294967295ul)
	func = ndfunc_alloc( bit_count_false_int64_t_loop, HAS_LOOP,
				1, 1, cBit, cInt64 );
    else
      func = ndfunc_alloc( bit_count_false_int32_t_loop, HAS_LOOP,
			      1, 1, cBit, cInt32 );
    func->args[1].init = INT2FIX(0);
    v = ndfunc_execute_reduce( func, mark, 1, self );
    ndfunc_free(func);
    v = rb_funcall(v,rb_intern("extract"),0);
    return v;
}



typedef struct {
    size_t count;
    char  *idx0;
    char  *idx1;
    size_t elmsz;
} where_opt_t;


static void
 bit_where_loop( na_iterator_t *const itr, VALUE info )
{
    size_t  i;
    BIT_DIGIT *a;
    size_t  p;
    ssize_t s;
    size_t *idx;
    BIT_DIGIT x=0;
    char   *idx0, *idx1;
    size_t  count;
    size_t  e;
    where_opt_t *g;

    Data_Get_Struct(info, where_opt_t, g);
    count = g->count;
    idx0  = g->idx0;
    idx1  = g->idx1;
    e     = g->elmsz;
    INIT_COUNTER( itr, i );
    INIT_PTR_BIT( itr, 0, a, p, s, idx );
    if (idx) {
	for (; i--; ) {
	    LOAD_BIT( a, p+*idx, x );
	    idx++;
	    if (x==0) {
		if (idx0) {
		    STORE_INT(idx0,e,count);
		    idx0 += e;
		}
	    } else {
		if (idx1) {
		    STORE_INT(idx1,e,count);
		    idx1 += e;
		}
	    }
	    count++;
	}
    } else {
	for (; i--; ) {
	    LOAD_BIT( a, p, x );
	    p+=s;
	    if (x==0) {
		if (idx0) {
		    STORE_INT(idx0,e,count);
		    idx0 += e;
		}
	    } else {
		if (idx1) {
		    STORE_INT(idx1,e,count);
		    idx1 += e;
		}
	    }
	    count++;
	}
    }
    g->count = count;
    g->idx0  = idx0;
    g->idx1  = idx1;
}


static VALUE
 num_bit_where( VALUE self )
{
    VALUE idx_1, opt;
    ndfunc_t *func;
    size_t size, n_1;
    where_opt_t *g;

    func = ndfunc_alloc( bit_where_loop, HAS_LOOP, 1, 0, cBit );

    //self = na_flatten(self);
    size = RNARRAY_SIZE(self);
    n_1 = NUM2SIZE( num_bit_count_true( 0, NULL, self ) );
    g = ALLOC(where_opt_t);
    g->count = 0;
    if (size>4294967295ul) {
	idx_1 = rb_narray_new( cInt64, 1, &n_1 );
	g->elmsz = 8;
    } else {
	idx_1 = rb_narray_new( cInt32, 1, &n_1 );
	g->elmsz = 4;
    }
    g->idx1 = na_data_pointer_for_write(idx_1);
    g->idx0 = NULL;
    opt = Data_Wrap_Struct(rb_cData,0,0,g);
    ndfunc_execute( func, 2, self, opt );
    ndfunc_free(func);
    return idx_1;
}



static VALUE
 num_bit_where2( VALUE self )
{
    VALUE idx_1, idx_0, opt;
    ndfunc_t *func;
    size_t size, n_1;
    where_opt_t *g;

    func = ndfunc_alloc( bit_where_loop, HAS_LOOP, 1, 0, cBit );

    //self = na_flatten(self);
    size = RNARRAY_SIZE(self);
    n_1 = NUM2SIZE( num_bit_count_true( 0, NULL, self ) );
    g = ALLOC(where_opt_t);
    g->count = 0;
    if (size>4294967295ul) {
	idx_1 = rb_narray_new( cInt64, 1, &n_1 );
	idx_0 = rb_narray_new( cInt64, 1, &n_1 );
	g->elmsz = 8;
    } else {
	idx_1 = rb_narray_new( cInt32, 1, &n_1 );
	idx_0 = rb_narray_new( cInt32, 1, &n_1 );
	g->elmsz = 4;
    }
    g->idx1 = na_data_pointer_for_write(idx_1);
    g->idx0 = na_data_pointer_for_write(idx_0);
    opt = Data_Wrap_Struct(rb_cData,0,0,g);
    ndfunc_execute( func, 2, self, opt );
    ndfunc_free(func);
    return rb_assoc_new(idx_1,idx_0);
}



static void
 bit_mask_loop( na_iterator_t *const itr, VALUE info )
{
    size_t  i;
    BIT_DIGIT *a1;
    size_t  p1, q1;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    BIT_DIGIT x=0;
    char    *p2, *q2, *q3;
    size_t  e2;
    void  **g;

    Data_Get_Struct(info, void*, g);
    q3 = *g;
    INIT_COUNTER( itr, i );
    INIT_PTR_BIT( itr, 0, a1, p1, s1, idx1 );
    INIT_PTR_ELM( itr, 1, p2, s2, idx2, e2 );
    for (; i--; ) {
      if (idx1) {
	q1 = p1+*idx1;
	idx1++;
      } else {
	q1 = p1;
	p1 += s1;
      }
      if (idx2) {
	q2 = p2+*idx2;
	idx2++;
      } else {
	q2 = p2;
	p2 += s2;
      }
      LOAD_BIT( a1, q1, x );
      if (x!=0) {
	memcpy(q3,q2,e2);
	q3 += e2;
      }
    }
    *g = q3;
}


static VALUE
 num_bit_mask( VALUE mask, VALUE val )
{
    VALUE result, opt;
    VALUE vfalse = Qfalse;
    ndfunc_t *func;
    size_t size;
    void **g;

    size = NUM2SIZE( num_bit_count_true( 0, NULL, mask ) );
    result = rb_narray_new( CLASS_OF(val), 1, &size );
    g = ALLOC(void *);
    *g = na_data_pointer_for_write(result);
    opt = Data_Wrap_Struct(rb_cData,0,0,g);
    func = ndfunc_alloc( bit_mask_loop, HAS_LOOP, 2, 0, cBit, Qnil );
    ndfunc_execute(func, 3, mask, val, opt);
    ndfunc_free(func);
    return result;
}



void
Init_bit()
{
    volatile VALUE hCast;

    rb_define_const(cBit, "ELEMENT_BIT_SIZE",  INT2FIX(1));
    rb_define_const(cBit, "ELEMENT_BYTE_SIZE", rb_float_new(1.0/8));
    rb_define_const(cBit, "CONTIGUOUS_STRIDE", INT2FIX(1));

    rb_define_singleton_method(cBit, "_cast", num_bit_s__cast, 1);

    rb_define_singleton_method(cBit, "bit_and", num_bit_s_and, 2);
    rb_define_singleton_method(cBit, "bit_or",  num_bit_s_or,  2);
    rb_define_singleton_method(cBit, "bit_xor", num_bit_s_xor, 2);

    rb_define_method(cBit, "~@", num_bit_not, 0);

    rb_define_method(cBit, "count_true", num_bit_count_true, -1);
    rb_define_alias (cBit, "count_1","count_true");
    rb_define_method(cBit, "count_false", num_bit_count_false, -1);
    rb_define_alias (cBit, "count_0","count_false");
    rb_define_method(cBit, "where", num_bit_where, 0);
    rb_define_method(cBit, "where2", num_bit_where2, 0);
    rb_define_method(cBit, "mask", num_bit_mask, 1);

    rb_define_method(cBit, "_cast_to", num_bit__cast_to, 1);
    rb_define_method(cBit, "debug_print", num_bit_debug_print, 0);
    rb_define_method(cBit, "inspect", num_bit_inspect, 0);
    rb_define_method(cBit, "format", num_bit_format, -1);
    rb_define_method(cBit, "format_to_a", num_bit_format_to_a, -1);

    rb_define_method(cBit, "fill", num_bit_fill, 1);

    rb_define_method(cBit, "to_a", num_bit_cast_to_rarray, 0);

    rb_define_method(cBit, "extract", num_bit_extract, 0);

    rb_define_method(cBit, "copy",  num_bit_copy, 0);
    rb_define_method(cBit, "store", num_bit_store, 1);
    rb_define_method(cBit, "[]=",   num_bit_aset, -1);

    hCast = rb_hash_new();
    rb_define_const(cBit, "UPCAST", hCast);
    rb_hash_aset(hCast, cInt32, cInt32);
    rb_hash_aset(hCast, cInt16, cInt16);
    rb_hash_aset(hCast, cInt8,  cInt8 );
}
