/*
  int32.c
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
 if      (klass==cInt32 || klass==cUInt32) esz=4; \
 else if (klass==cInt8  || klass==cUInt8 ) esz=1; \
 else if (klass==cInt16 || klass==cUInt16) esz=2; \
 else if (klass==cInt24 || klass==cUInt24) esz=3; \
 else rb_raise(rb_eRuntimeError,"unexpected class: %s",rb_obj_classname(klass));}

#include "template.h"
#include "template_int.h"
#include "template_data.h"
#include "template_sort.h"
#include "SFMT.h"

// Int32..Int8 --> Int32..Int8
static void
cast_int32_to_int32( na_iterator_t *const itr, VALUE info )
{
    size_t     i;
    ssize_t    s1, s2;
    size_t    *idx1, *idx2;
    char      *p1, *p2;
    int        e1, e2;
    int32_t    x;

    INIT_COUNTER( itr, i );
    INIT_PTR_ELM( itr, 0, p1, s1, idx1, e1 );
    INIT_PTR_ELM( itr, 1, p2, s2, idx2, e2 );
    if (idx1||idx2)
	for (; i--; ) {
	    LOAD_INT_STEP( p1, s1, idx1, e1, int32_t, x );
	    STORE_INT_STEP( p2, s2, idx2, e2, int32_t, x );
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
num_cast_int32_to_int32( VALUE klass, VALUE self )
{
    VALUE v;
    ndfunc_t *func;

    func = ndfunc_alloc( cast_int32_to_int32, HAS_LOOP, 1, 1, Qnil, klass );
    v = ndfunc_execute( func, 1, self );
    ndfunc_free( func );
    return v;
}


// Bit --> Int32..Int8
static void
int32_cast_bit( na_iterator_t *const itr, VALUE info )
{
    size_t     i;
    ssize_t    s1, s2;
    size_t    *idx1, *idx2;
    BIT_DIGIT *a1;
    size_t     p1;
    char      *p2;
    int        e2;
    BIT_DIGIT  x;
    int32_t    y;

    INIT_COUNTER( itr, i );
    INIT_PTR_BIT( itr, 0, a1, p1, s1, idx1 );
    INIT_PTR_ELM( itr, 1, p2, s2, idx2, e2 );
    if (idx1||idx2)
	for (; i--; ) {
	    LOAD_BIT_STEP( a1, p1, s1, idx1, x );
	    y = x;
	    STORE_INT_STEP( p2, s2, idx2, e2, int32_t, y );
	}
    else
	for (; i--; ) {
	    LOAD_BIT(a1,p1,x);
	    p1+=s1;
	    //printf("x=%d\n",x);
	    y = x;
	    STORE_INT(p2,e2,y);
	    p2+=s2;
	}
}

VALUE
num_cast_bit_to_int32( VALUE klass, VALUE self )
{
    VALUE v;
    ndfunc_t *func;

    func = ndfunc_alloc( int32_cast_bit, HAS_LOOP, 1, 1, cBit, klass );
    v = ndfunc_execute( func, 1, self );
    ndfunc_free( func );
    return v;
}

static VALUE
 num_int32__cast_to( VALUE value, VALUE type )
{ return Qnil; }

FILL_INT_WITH_NUM( int32, int32_t, y=NUM2INT(x) )

CAST_DATA_TO_INT( dfloat, double, int32, int32_t )
CAST_INT_TO_NUM( int32, int32_t, INT2NUM )
CAST_NUM_TO_INT( int32, int32_t, NUM2INT )
CAST_INT_TO_ARRAY( int32, int32_t, INT2NUM )
CAST_ARRAY_TO_INT( int32, int32_t, NUM2INT )

VALUE num_cast_int64_to_int64( VALUE klass, VALUE self );

static VALUE
num_int32_s__cast(VALUE klass, VALUE obj)
{
    VALUE type;

    type = CLASS_OF(obj);
    if (type==klass) {
	return obj;
    } else if (type==cDFloat) {
	return num_cast_dfloat_to_int32(klass, obj);
    } else if (rb_obj_is_kind_of(obj,cInt32)) {
	return num_cast_int32_to_int32(klass, obj);
    } else if (rb_obj_is_kind_of(obj,cInt64)) {
	return num_cast_int64_to_int64(klass, obj);
    } else if (rb_obj_is_kind_of(obj,cBit)) {
	return num_cast_bit_to_int32(klass, obj);
    } else if (TYPE(obj)==T_ARRAY) {
	return num_cast_array_to_int32(klass, obj);
    } else if ( TYPE(obj)==T_FLOAT || FIXNUM_P(obj) || TYPE(obj)==T_BIGNUM ) {
	return num_cast_numeric_to_int32(klass, obj);
    }
    return Qnil;
}

DEBUG_PRINT_INT(int32, 4, long, "%ld")
DEBUG_PRINT_INT(int16, 2, long, "%ld")
DEBUG_PRINT_INT(int8,  1, long, "%ld")
DEBUG_PRINT_INT(int24, 3, long, "%ld")

FORMAT_INT(int32,int32_t,4,16,"%"INT32FMT"d")
FORMAT_INT(int24,int32_t,3,16,"%"INT32FMT"d")
FORMAT_INT(int16,int32_t,2,16,"%"INT32FMT"d")
FORMAT_INT(int8, int32_t,1,16,"%"INT32FMT"d")

INSPECT_FUNC(int32)
INSPECT_FUNC(int16)
INSPECT_FUNC(int8)
INSPECT_FUNC(int24)

FORMAT_FUNC(int32)
FORMAT_FUNC(int16)
FORMAT_FUNC(int8)
FORMAT_FUNC(int24)

BINARY_INT_FUNC(int32,cInt32,int32_t,add,x+=y)
BINARY_INT_FUNC(int32,cInt32,int32_t,sub,x-=y)
BINARY_INT_FUNC(int32,cInt32,int32_t,mul,x*=y)
BINARY_INT_FUNC(int32,cInt32,int32_t,div,x/=y)
BINARY_INT_FUNC(int32,cInt32,int32_t,mod,x%=y)

static int32_t powint_int32(int32_t x, int p)
{
  int32_t r=1;
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
BINARY_INT_FUNC(int32,cInt32,int32_t,pow,x=powint_int32(x,y))

BINARY_INT_FUNC(int32,cInt32,int32_t,bit_and,x&=y)
BINARY_INT_FUNC(int32,cInt32,int32_t,bit_or, x|=y)
BINARY_INT_FUNC(int32,cInt32,int32_t,bit_xor,x^=y)

UNARY_INT_FUNC(int32,cInt32,int32_t,abs, if (x<0) x=-x;)
UNARY_INT_FUNC(int32,cInt32,int32_t,minus, x=-x)
UNARY_INT_FUNC(int32,cInt32,int32_t,bit_not, x=~x)

CMP_BINARY_INT_FUNC(int32,cInt32,int32_t,eq,x==y)
CMP_BINARY_INT_FUNC(int32,cInt32,int32_t,ne,x!=y)
CMP_BINARY_INT_FUNC(int32,cInt32,int32_t,gt,x>y)
CMP_BINARY_INT_FUNC(int32,cInt32,int32_t,ge,x>=y)
CMP_BINARY_INT_FUNC(int32,cInt32,int32_t,lt,x<y)
CMP_BINARY_INT_FUNC(int32,cInt32,int32_t,le,x<=y)

ACCUM_UNARY_INT_FUNC(int32,cInt32,int32_t,sum,
		{func->args[1].init=INT2FIX(0);},
		 (x!=0), {x+=y;} )
ACCUM_UNARY_INT_FUNC(int32,cInt32,int32_t,accum,
		{func->args[1].init = INT2FIX(0); NDF_SET(func,NDF_KEEP_DIM);},
		 (x!=0), {x+=y;} )

ACCUM_UNARY_INT_FUNC(int32,cInt32,int32_t,min,
		 {func->args[1].init = num_init_accum_aref0( self, mark );},
		 y>x, {} )
ACCUM_UNARY_INT_FUNC(int32,cInt32,int32_t,max,
		 {func->args[1].init = num_init_accum_aref0( self, mark );},
		 y<x, {} )

INT_SEQ(int32,int32_t)

RAND_INT_FUNC(int32,int32_t,gen_rand32,32,1)


#define cmp(a,b)					\
    ( (*(int32_t*)(a)==*(int32_t*)(b)) ? 0 :		\
      (*(int32_t*)(a) > *(int32_t*)(b)) ? 1 : -1 )
#define cmpgt(a,b)				\
    (*(int32_t*)(a) > *(int32_t*)(b))

QSORT_FUNC(int32,int32_t)
SORT_FUNC(int32,cInt32,int32_t)

#undef cmp
#undef cmpgt
#define cmp(a,b)					\
    ( (**(int32_t**)(a)==**(int32_t**)(b)) ? 0 :		\
      (**(int32_t**)(a) > **(int32_t**)(b)) ? 1 : -1 )
#define cmpgt(a,b)				\
    (**(int32_t**)(a) > **(int32_t**)(b))

QSORT_FUNC(int32_index,int32_t*)
SORT_INDEX_FUNC(int32,cInt32,int32_t)

MEDIAN_FUNC(int32,cInt32,int32_t)


static VALUE 
num_int32_self( VALUE self )
{ return self; }

void
Init_int32()
{
    volatile VALUE hCast;

    rb_define_const(cInt32, ELEMENT_BIT_SIZE,  INT2FIX(32));
    rb_define_const(cInt32, ELEMENT_BYTE_SIZE, INT2FIX(4));
    rb_define_const(cInt32, CONTIGUOUS_STRIDE, INT2FIX(4));
    rb_define_const(cInt8,  ELEMENT_BIT_SIZE,  INT2FIX(8));
    rb_define_const(cInt8,  ELEMENT_BYTE_SIZE, INT2FIX(1));
    rb_define_const(cInt8,  CONTIGUOUS_STRIDE, INT2FIX(1));
    rb_define_const(cInt16, ELEMENT_BIT_SIZE,  INT2FIX(16));
    rb_define_const(cInt16, ELEMENT_BYTE_SIZE, INT2FIX(2));
    rb_define_const(cInt16, CONTIGUOUS_STRIDE, INT2FIX(2));
    rb_define_const(cInt24, ELEMENT_BIT_SIZE,  INT2FIX(24));
    rb_define_const(cInt24, ELEMENT_BYTE_SIZE, INT2FIX(3));
    rb_define_const(cInt24, CONTIGUOUS_STRIDE, INT2FIX(3));

    rb_define_singleton_method(cInt32, "_cast", num_int32_s__cast, 1);

    rb_define_singleton_method(cInt32, "add", num_int32_s_add, 2);
    rb_define_singleton_method(cInt32, "sub", num_int32_s_sub, 2);
    rb_define_singleton_method(cInt32, "mul", num_int32_s_mul, 2);
    rb_define_singleton_method(cInt32, "div", num_int32_s_div, 2);
    rb_define_singleton_method(cInt32, "mod", num_int32_s_mod, 2);
    rb_define_singleton_method(cInt32, "pow", num_int32_s_pow, 2);

    rb_define_singleton_method(cInt32, "bit_and", num_int32_s_bit_and, 2);
    rb_define_singleton_method(cInt32, "bit_or",  num_int32_s_bit_or,  2);
    rb_define_singleton_method(cInt32, "bit_xor", num_int32_s_bit_xor, 2);
    rb_define_method(cInt32, "~@", num_int32_bit_not, 0);

    rb_define_singleton_method(cInt32, "eq", num_int32_s_eq, 2);
    rb_define_singleton_method(cInt32, "ne", num_int32_s_ne, 2);
    rb_define_singleton_method(cInt32, "gt", num_int32_s_gt, 2);
    rb_define_singleton_method(cInt32, "ge", num_int32_s_ge, 2);
    rb_define_singleton_method(cInt32, "lt", num_int32_s_lt, 2);
    rb_define_singleton_method(cInt32, "le", num_int32_s_le, 2);

    rb_define_method(cInt32, "-@", num_int32_minus, 0);
    rb_define_method(cInt32, "abs",   num_int32_abs,  0);
    rb_define_method(cInt32, "floor", num_int32_self, 0);
    rb_define_method(cInt32, "round", num_int32_self, 0);
    rb_define_method(cInt32, "ceil",  num_int32_self, 0);

    rb_define_method(cInt32, "sum", num_int32_sum, -1);
    rb_define_method(cInt32, "accum", num_int32_accum, -1);
    rb_define_method(cInt32, "min", num_int32_min, -1);
    rb_define_method(cInt32, "max", num_int32_max, -1);

    rb_define_method(cInt32, "seq", num_int32_seq, -1);

    rb_define_method(cInt32, "sort", num_int32_sort, -1);
    rb_define_method(cInt32, "sort!", num_int32_sort_bang, -1);
    rb_define_method(cInt32, "sort_index", num_int32_sort_index, -1);
    rb_define_method(cInt32, "median", num_int32_median, -1);

    rb_define_method(cInt32, "_cast_to", num_int32__cast_to, 1);
    rb_define_method(cInt32, "fill", num_int32_fill, 1);
    rb_define_method(cInt32, "rand", num_int32_rand, -1);
    rb_define_method(cInt32, "debug_print", num_int32_debug_print, 0);
    rb_define_method(cInt24, "debug_print", num_int24_debug_print, 0);
    rb_define_method(cInt16, "debug_print", num_int16_debug_print, 0);
    rb_define_method(cInt8,  "debug_print", num_int8_debug_print,  0);

    rb_define_method(cInt32, "inspect", num_int32_inspect, 0);
    rb_define_method(cInt32, "format", num_int32_format, -1);
    rb_define_method(cInt32, "format_to_a", num_int32_format_to_a, -1);
    rb_define_method(cInt24, "inspect", num_int24_inspect, 0);
    rb_define_method(cInt24, "format", num_int24_format, -1);
    rb_define_method(cInt24, "format_to_a", num_int24_format_to_a, -1);
    rb_define_method(cInt16, "inspect", num_int16_inspect, 0);
    rb_define_method(cInt16, "format", num_int16_format, -1);
    rb_define_method(cInt16, "format_to_a", num_int16_format_to_a, -1);
    rb_define_method(cInt8,  "inspect", num_int8_inspect, 0);
    rb_define_method(cInt8,  "format", num_int8_format, -1);
    rb_define_method(cInt8,  "format_to_a", num_int8_format_to_a, -1);

    rb_define_method(cInt32, "to_a", num_cast_int32_to_array, 0);

    rb_define_method(cInt32, "extract", num_int32_extract, 0);

    hCast = rb_hash_new();
    rb_define_const(cInt32, "UPCAST", hCast);
    rb_hash_aset(hCast, cInt64,      cInt64);
    rb_hash_aset(hCast, cInt48,      cInt48);
    rb_hash_aset(hCast, cInt32,      cInt32);
    rb_hash_aset(hCast, cInt24,      cInt32);
    rb_hash_aset(hCast, cInt16,      cInt32);
    rb_hash_aset(hCast, cInt8,       cInt32);
    rb_hash_aset(hCast, cUInt64,     cInt64);
    rb_hash_aset(hCast, cUInt48,     cInt48);
    rb_hash_aset(hCast, cUInt32,     cInt32);
    rb_hash_aset(hCast, cUInt24,     cInt32);
    rb_hash_aset(hCast, cUInt16,     cInt32);
    rb_hash_aset(hCast, cUInt8,      cInt32);
    rb_hash_aset(hCast, rb_cArray,   cInt32);
    rb_hash_aset(hCast, cDComplex,   cDComplex);
    rb_hash_aset(hCast, rb_cFixnum,  cInt32);
    rb_hash_aset(hCast, rb_cBignum,  cInt32);
    rb_hash_aset(hCast, rb_cInteger, cInt32);
    rb_hash_aset(hCast, rb_cFloat,   cDFloat);

    hCast = rb_hash_new();
    rb_define_const(cInt24, "UPCAST", hCast);
    rb_hash_aset(hCast, cInt32,      cInt32);
    rb_hash_aset(hCast, cInt24,      cInt24);
    rb_hash_aset(hCast, cInt16,      cInt24);
    rb_hash_aset(hCast, cInt8,       cInt24);
    rb_hash_aset(hCast, cUInt32,     cInt32);
    rb_hash_aset(hCast, cUInt24,     cInt24);
    rb_hash_aset(hCast, cUInt16,     cInt24);
    rb_hash_aset(hCast, cUInt8,      cInt24);
    rb_hash_aset(hCast, rb_cArray,   cInt24);
    rb_hash_aset(hCast, cDComplex,   cDComplex);
    rb_hash_aset(hCast, rb_cFixnum,  cInt24);
    rb_hash_aset(hCast, rb_cBignum,  cInt24);
    rb_hash_aset(hCast, rb_cInteger, cInt24);
    rb_hash_aset(hCast, rb_cFloat,   cDFloat);

    hCast = rb_hash_new();
    rb_define_const(cInt16, "UPCAST", hCast);
    rb_hash_aset(hCast, cInt32,      cInt32);
    rb_hash_aset(hCast, cInt24,      cInt24);
    rb_hash_aset(hCast, cInt16,      cInt16);
    rb_hash_aset(hCast, cInt8,       cInt16);
    rb_hash_aset(hCast, cUInt32,     cInt32);
    rb_hash_aset(hCast, cUInt24,     cInt24);
    rb_hash_aset(hCast, cUInt16,     cInt16);
    rb_hash_aset(hCast, cUInt8,      cInt16);
    rb_hash_aset(hCast, rb_cArray,   cInt16);
    rb_hash_aset(hCast, cDComplex,   cDComplex);
    rb_hash_aset(hCast, rb_cFixnum,  cInt16);
    rb_hash_aset(hCast, rb_cBignum,  cInt16);
    rb_hash_aset(hCast, rb_cInteger, cInt16);
    rb_hash_aset(hCast, rb_cFloat,   cDFloat);
    rb_hash_aset(hCast, rb_cArray,   cInt8);
    rb_hash_aset(hCast, rb_cArray,   cInt8);

    hCast = rb_hash_new();
    rb_define_const(cInt8, "UPCAST", hCast);
    rb_hash_aset(hCast, cInt32,      cInt32);
    rb_hash_aset(hCast, cInt24,      cInt24);
    rb_hash_aset(hCast, cInt16,      cInt16);
    rb_hash_aset(hCast, cInt8,       cInt8);
    rb_hash_aset(hCast, cUInt32,     cInt32);
    rb_hash_aset(hCast, cUInt24,     cInt24);
    rb_hash_aset(hCast, cUInt16,     cInt16);
    rb_hash_aset(hCast, cUInt8,      cInt8);
    rb_hash_aset(hCast, rb_cArray,   cInt8);
    rb_hash_aset(hCast, cDComplex,   cDComplex);
    rb_hash_aset(hCast, rb_cFixnum,  cInt8);
    rb_hash_aset(hCast, rb_cBignum,  cInt8);
    rb_hash_aset(hCast, rb_cInteger, cInt8);
    rb_hash_aset(hCast, rb_cFloat,   cDFloat);
}
