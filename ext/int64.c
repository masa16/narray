/*
  int64.c
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

#ifdef WORDS_BIGENDIAN
#define LOAD_INT( ptr, esz, x ) \
  { x=((char*)(ptr))[0]; memcpy(((char*)&(x))+(sizeof(x)-(esz)),ptr,esz); } // big endian
#define STORE_INT( ptr, esz, x ) \
  memcpy(ptr,((char*)&(x))+(sizeof(x)-(esz)),esz) // big endian
#else
#define LOAD_INT( ptr, esz, x )					\
  { x=((char*)(ptr))[(esz)-1]; memcpy(&(x),ptr,esz); } // little endian
#define STORE_INT( ptr, esz, x ) \
  memcpy(ptr,&(x),esz) // little endian
#endif

#define SET_INT_ELEMENT_SIZE(klass,esz) { \
 if      (klass==cInt64 || klass==cUInt64) esz=8; \
 else if (klass==cInt48 || klass==cUInt48) esz=6; \
 else rb_raise(rb_eRuntimeError,"unexpected class: %s",rb_obj_classname(klass));}

#include "template.h"
#include "template_int.h"
#include "template_data.h"
#include "template_sort.h"
#include "SFMT.h"

// Int64 --> Int64
static void
cast_int64_to_int64( na_iterator_t *const itr, VALUE info )
{
    size_t     i;
    ssize_t    s1, s2;
    size_t    *idx1, *idx2;
    char      *p1, *p2;
    int        e1, e2;
    int64_t    x;

    INIT_COUNTER( itr, i );
    INIT_PTR_ELM( itr, 0, p1, s1, idx1, e1 );
    INIT_PTR_ELM( itr, 1, p2, s2, idx2, e2 );
    if (idx1||idx2)
	for (; i--; ) {
	    LOAD_INT_STEP( p1, s1, idx1, e1, int64_t, x );
	    STORE_INT_STEP( p2, s2, idx2, e2, int64_t, x );
	}
    else
	for (; i--; ) {
	    LOAD_INT(p1,e1,x);
	    p1+=s1;
	    STORE_INT(p2,e2,x);
	    p2+=s2;
	}
}

VALUE
num_cast_int64_to_int64( VALUE klass, VALUE self )
{
    VALUE v;
    ndfunc_t *func;

    func = ndfunc_alloc( cast_int64_to_int64, HAS_LOOP, 1, 1, CLASS_OF(self), klass );
    v = ndfunc_execute( func, 1, self );
    ndfunc_free( func );
    return v;
}


static VALUE
 num_int64__cast_to( VALUE value, VALUE type )
{ return Qnil; }

FILL_INT_WITH_NUM( int64, int64_t, y=NUM2INT64(x) )

CAST_DATA_TO_INT( dfloat, double, int64, int64_t )
CAST_INT_TO_NUM( int64, int64_t, INT642NUM )
CAST_NUM_TO_INT( int64, int64_t, NUM2INT64 )
CAST_INT_TO_ARRAY( int64, int64_t, INT642NUM )
CAST_ARRAY_TO_INT( int64, int64_t, NUM2INT64 )

static VALUE
num_int64_s__cast(VALUE klass, VALUE obj)
{
    VALUE type;

    type = CLASS_OF(obj);
    if (type==klass) {
	return obj;
    } else if (type==cDFloat) {
	return num_cast_dfloat_to_int64(klass, obj);
    } else if (rb_obj_is_kind_of(obj,cInt64)) {
	return num_cast_int64_to_int64(klass, obj);
    } else if (rb_obj_is_kind_of(obj,cInt32)) {
	return num_cast_int64_to_int64(klass, obj);
    } else if (TYPE(obj)==T_ARRAY) {
	return num_cast_array_to_int64(klass, obj);
    } else if ( TYPE(obj)==T_FLOAT || FIXNUM_P(obj) || TYPE(obj)==T_BIGNUM ) {
	return num_cast_numeric_to_int64(klass, obj);
    }
    return Qnil;
}

DEBUG_PRINT_INT(int64, 8, long long, "%lld")
DEBUG_PRINT_INT(int48, 6, long, "%ld")

FORMAT_INT(int64,int64_t,8,24,"%"INT64FMT"d")
FORMAT_INT(int48,int64_t,6,24,"%"INT64FMT"d")
INSPECT_FUNC(int64)
INSPECT_FUNC(int48)
FORMAT_FUNC(int64)
FORMAT_FUNC(int48)

BINARY_INT_FUNC(int64,cInt64,int64_t,add,x+=y)
BINARY_INT_FUNC(int64,cInt64,int64_t,sub,x-=y)
BINARY_INT_FUNC(int64,cInt64,int64_t,mul,x*=y)
BINARY_INT_FUNC(int64,cInt64,int64_t,div,x/=y)
BINARY_INT_FUNC(int64,cInt64,int64_t,mod,x%=y)

static int64_t powint_int64(int64_t x, int p)
{
  int64_t r=1;
  switch(p) {
  case 2: return x*x;
  case 3: return x*x*x;
  case 1: return x;
  case 0: return 1;
  }
  if (p<0)  return 0;
  while (p) {
    if ( (p%2) == 1 ) r *= x;
    x *= x;
    p /= 2;
  }
  return r;
}
BINARY_INT_FUNC(int64,cInt64,int64_t,pow,x=powint_int64(x,y))

BINARY_INT_FUNC(int64,cInt64,int64_t,bit_and,x&=y)
BINARY_INT_FUNC(int64,cInt64,int64_t,bit_or, x|=y)
BINARY_INT_FUNC(int64,cInt64,int64_t,bit_xor,x^=y)

UNARY_INT_FUNC(int64,cInt64,int64_t,abs, if (x<0) x=-x;)
UNARY_INT_FUNC(int64,cInt64,int64_t,minus, x=-x)
UNARY_INT_FUNC(int64,cInt64,int64_t,bit_not, x=~x)

CMP_BINARY_INT_FUNC(int64,cInt64,int64_t,eq,x==y)
CMP_BINARY_INT_FUNC(int64,cInt64,int64_t,ne,x!=y)
CMP_BINARY_INT_FUNC(int64,cInt64,int64_t,gt,x>y)
CMP_BINARY_INT_FUNC(int64,cInt64,int64_t,ge,x>=y)
CMP_BINARY_INT_FUNC(int64,cInt64,int64_t,lt,x<y)
CMP_BINARY_INT_FUNC(int64,cInt64,int64_t,le,x<=y)

ACCUM_UNARY_INT_FUNC(int64,cInt64,int64_t,sum,
		{func->args[1].init=INT2FIX(0);},
		 (x!=0), {x+=y;} )
ACCUM_UNARY_INT_FUNC(int64,cInt64,int64_t,accum,
		{func->args[1].init = INT2FIX(0); NDF_SET(func,NDF_KEEP_DIM);},
		 (x!=0), {x+=y;} )

ACCUM_UNARY_INT_FUNC(int64,cInt64,int64_t,min,
		 {func->args[1].init = num_init_accum_aref0( self, mark );},
		 y>x, {} )
ACCUM_UNARY_INT_FUNC(int64,cInt64,int64_t,max,
		 {func->args[1].init = num_init_accum_aref0( self, mark );},
		 y<x, {} )

INT_SEQ(int64,int64_t)
RAND_INT_FUNC(int64,int64_t,gen_rand64,64,1)

#define cmp(a,b)				\
    ( (*(int64_t*)(a)==*(int64_t*)(b)) ? 0 :	\
      (*(int64_t*)(a) > *(int64_t*)(b)) ? 1 : -1 )
#define cmpgt(a,b)				\
    (*(int64_t*)(a) > *(int64_t*)(b))
QSORT_FUNC(int64,int64_t)
SORT_FUNC(int64,cInt64,int64_t)

#undef cmp
#undef cmpgt
#define cmp(a,b)				\
    ( (**(int64_t**)(a)==**(int64_t**)(b)) ? 0 :\
      (**(int64_t**)(a) > **(int64_t**)(b)) ? 1 : -1 )
#define cmpgt(a,b)				\
    (**(int64_t**)(a) > **(int64_t**)(b))
QSORT_FUNC(int64_index,int64_t*)
SORT_INDEX_FUNC(int64,cInt64,int64_t)
MEDIAN_FUNC(int64,cInt64,int64_t)

static VALUE 
num_int64_self( VALUE self )
{ return self; }

void
Init_int64()
{
    volatile VALUE hCast;

    rb_define_const(cInt64, ELEMENT_BIT_SIZE,  INT2FIX(8*8));
    rb_define_const(cInt64, ELEMENT_BYTE_SIZE, INT2FIX(8));
    rb_define_const(cInt64, CONTIGUOUS_STRIDE, INT2FIX(8));
    rb_define_const(cInt48, ELEMENT_BIT_SIZE,  INT2FIX(6*8));
    rb_define_const(cInt48, ELEMENT_BYTE_SIZE, INT2FIX(6));
    rb_define_const(cInt48, CONTIGUOUS_STRIDE, INT2FIX(6));

    rb_define_singleton_method(cInt64, "_cast", num_int64_s__cast, 1);

    rb_define_singleton_method(cInt64, "add", num_int64_s_add, 2);
    rb_define_singleton_method(cInt64, "sub", num_int64_s_sub, 2);
    rb_define_singleton_method(cInt64, "mul", num_int64_s_mul, 2);
    rb_define_singleton_method(cInt64, "div", num_int64_s_div, 2);
    rb_define_singleton_method(cInt64, "mod", num_int64_s_mod, 2);
    rb_define_singleton_method(cInt64, "pow", num_int64_s_pow, 2);

    rb_define_singleton_method(cInt64, "bit_and", num_int64_s_bit_and, 2);
    rb_define_singleton_method(cInt64, "bit_or",  num_int64_s_bit_or,  2);
    rb_define_singleton_method(cInt64, "bit_xor", num_int64_s_bit_xor, 2);
    rb_define_method(cInt64, "~@", num_int64_bit_not, 0);

    rb_define_singleton_method(cInt64, "eq", num_int64_s_eq, 2);
    rb_define_singleton_method(cInt64, "ne", num_int64_s_ne, 2);
    rb_define_singleton_method(cInt64, "gt", num_int64_s_gt, 2);
    rb_define_singleton_method(cInt64, "ge", num_int64_s_ge, 2);
    rb_define_singleton_method(cInt64, "lt", num_int64_s_lt, 2);
    rb_define_singleton_method(cInt64, "le", num_int64_s_le, 2);

    rb_define_method(cInt64, "-@", num_int64_minus, 0);
    rb_define_method(cInt64, "abs",   num_int64_abs,  0);
    rb_define_method(cInt64, "floor", num_int64_self, 0);
    rb_define_method(cInt64, "round", num_int64_self, 0);
    rb_define_method(cInt64, "ceil",  num_int64_self, 0);

    rb_define_method(cInt64, "sum", num_int64_sum, -1);
    rb_define_method(cInt64, "accum", num_int64_accum, -1);
    rb_define_method(cInt64, "min", num_int64_min, -1);
    rb_define_method(cInt64, "max", num_int64_max, -1);

    rb_define_method(cInt64, "seq", num_int64_seq, -1);
    rb_define_method(cInt64, "rand", num_int64_rand, -1);

    rb_define_method(cInt64, "sort", num_int64_sort, -1);
    rb_define_method(cInt64, "sort!", num_int64_sort_bang, -1);
    rb_define_method(cInt64, "sort_index", num_int64_sort_index, -1);
    rb_define_method(cInt64, "median", num_int64_median, -1);

    rb_define_method(cInt64, "_cast_to", num_int64__cast_to, 1);
    rb_define_method(cInt64, "fill", num_int64_fill, 1);
    rb_define_method(cInt64, "debug_print", num_int64_debug_print, 0);
    rb_define_method(cInt48, "debug_print", num_int48_debug_print, 0);

    rb_define_method(cInt64, "inspect", num_int64_inspect, 0);
    rb_define_method(cInt64, "format", num_int64_format, -1);
    rb_define_method(cInt64, "format_to_a", num_int64_format_to_a, -1);
    rb_define_method(cInt48, "inspect", num_int48_inspect, 0);
    rb_define_method(cInt48, "format", num_int48_format, -1);
    rb_define_method(cInt48, "format_to_a", num_int48_format_to_a, -1);

    rb_define_method(cInt64, "to_a", num_cast_int64_to_array, 0);

    rb_define_method(cInt64, "extract", num_int64_extract, 0);

    hCast = rb_hash_new();
    rb_define_const(cInt64, "UPCAST", hCast);
    rb_hash_aset(hCast, cInt64,      cInt64);
    rb_hash_aset(hCast, cInt48,      cInt64);
    rb_hash_aset(hCast, cInt32,      cInt64);
    rb_hash_aset(hCast, cInt24,      cInt64);
    rb_hash_aset(hCast, cInt16,      cInt64);
    rb_hash_aset(hCast, cInt8,       cInt64);
    rb_hash_aset(hCast, cUInt64,     cInt64);
    rb_hash_aset(hCast, cUInt48,     cInt64);
    rb_hash_aset(hCast, cUInt32,     cInt64);
    rb_hash_aset(hCast, cUInt24,     cInt64);
    rb_hash_aset(hCast, cUInt16,     cInt64);
    rb_hash_aset(hCast, cUInt8,      cInt64);
    rb_hash_aset(hCast, rb_cArray,   cInt64);
    rb_hash_aset(hCast, cDComplex,   cDComplex);
    rb_hash_aset(hCast, rb_cFixnum,  cInt64);
    rb_hash_aset(hCast, rb_cBignum,  cInt64);
    rb_hash_aset(hCast, rb_cInteger, cInt64);
    rb_hash_aset(hCast, rb_cFloat,   cDFloat);
}
