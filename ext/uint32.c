/*
  uint32.c
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
  { x=0; memcpy(((char*)&(x))+(sizeof(x)-(esz)),ptr,esz); } // big endian
#define STORE_INT( ptr, esz, x ) \
  memcpy(ptr,((char*)&(x))+(sizeof(x)-(esz)),esz) // big endian
#else
#define LOAD_INT( ptr, esz, x )	\
  { x=0; memcpy(&(x),ptr,esz); } // little endian
#define STORE_INT( ptr, esz, x ) \
  memcpy(ptr,&(x),esz) // little endian
#endif

#define SET_INT_ELEMENT_SIZE(klass,esz) { \
 if      (klass==cUInt32) esz=4; \
 else if (klass==cUInt8)  esz=1; \
 else if (klass==cUInt16) esz=2; \
 else if (klass==cUInt24) esz=3; \
 else rb_raise(rb_eRuntimeError,"unexpected class: %s",rb_obj_classname(klass));}

#include "template.h"
#include "template_int.h"
#include "template_data.h"
#include "template_sort.h"
#include "SFMT.h"


static VALUE
 num_uint32__cast_to( VALUE value, VALUE type )
{ return Qnil; }

FILL_INT_WITH_NUM( uint32, u_int32_t, y=NUM2UINT32(x) )

// DFloat --> Int32..Int8
CAST_DATA_TO_INT( dfloat, double, uint32, u_int32_t )
CAST_INT_TO_NUM( uint32, u_int32_t, UINT322NUM )
CAST_NUM_TO_INT( uint32, u_int32_t, NUM2UINT32 )
CAST_INT_TO_ARRAY( uint32, u_int32_t, UINT322NUM )
CAST_ARRAY_TO_INT( uint32, u_int32_t, NUM2UINT32 )

VALUE num_cast_int32_to_int32( VALUE klass, VALUE self );
VALUE num_cast_int32_to_bit( VALUE klass, VALUE self );

static VALUE
num_uint32_s__cast(VALUE klass, VALUE obj)
{
    VALUE type;

    type = CLASS_OF(obj);
    if (type==klass) {
	return obj;
    } else if (type==cDFloat) {
	return num_cast_dfloat_to_uint32(klass, obj);
    } else if (rb_obj_is_kind_of(obj,cInt32)) {
	return num_cast_int32_to_int32(klass, obj);
    } else if (rb_obj_is_kind_of(obj,cBit)) {
	return num_cast_bit_to_int32(klass, obj);
    } else if (TYPE(obj)==T_ARRAY) {
	return num_cast_array_to_uint32(klass, obj);
    } else if ( TYPE(obj)==T_FLOAT || FIXNUM_P(obj) || TYPE(obj)==T_BIGNUM ) {
	return num_cast_numeric_to_uint32(klass, obj);
    }
    return Qnil;
}

DEBUG_PRINT_INT(uint32, 4, unsigned long, "%lu")
DEBUG_PRINT_INT(uint16, 2, unsigned long, "%lu")
DEBUG_PRINT_INT(uint8,  1, unsigned long, "%lu")
DEBUG_PRINT_INT(uint24, 3, unsigned long, "%lu")

FORMAT_INT(uint32,u_int32_t,4,16,"%"INT32FMT"u")
FORMAT_INT(uint24,u_int32_t,3,16,"%"INT32FMT"u")
FORMAT_INT(uint16,u_int32_t,2,16,"%"INT32FMT"u")
FORMAT_INT(uint8, u_int32_t,1,16,"%"INT32FMT"u")

INSPECT_FUNC(uint32)
INSPECT_FUNC(uint16)
INSPECT_FUNC(uint8)
INSPECT_FUNC(uint24)

FORMAT_FUNC(uint32)
FORMAT_FUNC(uint16)
FORMAT_FUNC(uint8)
FORMAT_FUNC(uint24)

//BINARY_INT_FUNC(uint32,cUInt32,u_int32_t,add,x+=y)
//BINARY_INT_FUNC(uint32,cUInt32,u_int32_t,sub,x-=y)
//BINARY_INT_FUNC(uint32,cUInt32,u_int32_t,mul,x*=y)
BINARY_INT_FUNC(uint32,cUInt32,u_int32_t,div,x/=y)
BINARY_INT_FUNC(uint32,cUInt32,u_int32_t,mod,x%=y)

//UNARY_INT_FUNC(uint32,cUInt32,u_int32_t,minus,x=-x)

CMP_BINARY_INT_FUNC(uint32,cUInt32,u_int32_t,eq,x==y)
CMP_BINARY_INT_FUNC(uint32,cUInt32,u_int32_t,ne,x!=y)
CMP_BINARY_INT_FUNC(uint32,cUInt32,u_int32_t,gt,x>y)
CMP_BINARY_INT_FUNC(uint32,cUInt32,u_int32_t,ge,x>=y)
CMP_BINARY_INT_FUNC(uint32,cUInt32,u_int32_t,lt,x<y)
CMP_BINARY_INT_FUNC(uint32,cUInt32,u_int32_t,le,x<=y)

//ACCUM_UNARY_INT_FUNC(uint32,cUInt32,u_int32_t,sum,
//		{func->args[1].init=INT2FIX(0);},
//		 (x!=0), {x+=y;} )
//ACCUM_UNARY_INT_FUNC(uint32,cUInt32,u_int32_t,accum,
//		{func->args[1].init = INT2FIX(0); NDF_SET(func,NDF_KEEP_DIM);},
//		 (x!=0), {x+=y;} )

ACCUM_UNARY_INT_FUNC(uint32,cUInt32,u_int32_t,min,
		 {func->args[1].init = num_init_accum_aref0( self, mark );},
		 y>x, {} )
ACCUM_UNARY_INT_FUNC(uint32,cUInt32,u_int32_t,max,
		 {func->args[1].init = num_init_accum_aref0( self, mark );},
		 y<x, {} )

INT_SEQ(uint32,u_int32_t)
RAND_INT_FUNC(uint32,u_int32_t,gen_rand32,32,0)

#define cmp(a,b)					\
    ( (*(u_int32_t*)(a)==*(u_int32_t*)(b)) ? 0 :	\
      (*(u_int32_t*)(a) > *(u_int32_t*)(b)) ? 1 : -1 )
#define cmpgt(a,b)				\
    (*(u_int32_t*)(a) > *(u_int32_t*)(b))

QSORT_FUNC(uint32,u_int32_t)
SORT_FUNC(uint32,cUInt32,u_int32_t)

#undef cmp
#undef cmpgt
#define cmp(a,b)					\
    ( (**(u_int32_t**)(a)==**(u_int32_t**)(b)) ? 0 :	\
      (**(u_int32_t**)(a) > **(u_int32_t**)(b)) ? 1 : -1 )
#define cmpgt(a,b)				\
    (**(u_int32_t**)(a) > **(u_int32_t**)(b))

QSORT_FUNC(uint32_index,u_int32_t*)
SORT_INDEX_FUNC(uint32,cUInt32,u_int32_t)

MEDIAN_FUNC(uint32,cUInt32,u_int32_t)

static VALUE 
num_uint32_self( VALUE self )
{ return self;}

void
Init_uint32()
{
    volatile VALUE hCast;

    rb_define_const(cUInt32, ELEMENT_BIT_SIZE,  INT2FIX(4*8));
    rb_define_const(cUInt32, ELEMENT_BYTE_SIZE, INT2FIX(4));
    rb_define_const(cUInt32, CONTIGUOUS_STRIDE, INT2FIX(4));
    rb_define_const(cUInt8,  ELEMENT_BIT_SIZE,  INT2FIX(1*8));
    rb_define_const(cUInt8,  ELEMENT_BYTE_SIZE, INT2FIX(1));
    rb_define_const(cUInt8,  CONTIGUOUS_STRIDE, INT2FIX(1));
    rb_define_const(cUInt16, ELEMENT_BIT_SIZE,  INT2FIX(2*8));
    rb_define_const(cUInt16, ELEMENT_BYTE_SIZE, INT2FIX(2));
    rb_define_const(cUInt16, CONTIGUOUS_STRIDE, INT2FIX(2));
    rb_define_const(cUInt24, ELEMENT_BIT_SIZE,  INT2FIX(3*8));
    rb_define_const(cUInt24, ELEMENT_BYTE_SIZE, INT2FIX(3));
    rb_define_const(cUInt24, CONTIGUOUS_STRIDE, INT2FIX(3));

    rb_define_singleton_method(cUInt32, "_cast", num_uint32_s__cast, 1);

    //rb_define_singleton_method(cUInt32, "add", num_uint32_s_add, 2);
    //rb_define_singleton_method(cUInt32, "sub", num_uint32_s_sub, 2);
    //rb_define_singleton_method(cUInt32, "mul", num_uint32_s_mul, 2);
    rb_define_singleton_method(cUInt32, "div", num_uint32_s_div, 2);
    rb_define_singleton_method(cUInt32, "mod", num_uint32_s_mod, 2);
    //rb_define_singleton_method(cUInt32, "pow", num_uint32_s_pow, 2);

    rb_define_singleton_method(cUInt32, "eq", num_uint32_s_eq, 2);
    rb_define_singleton_method(cUInt32, "ne", num_uint32_s_ne, 2);
    rb_define_singleton_method(cUInt32, "gt", num_uint32_s_gt, 2);
    rb_define_singleton_method(cUInt32, "ge", num_uint32_s_ge, 2);
    rb_define_singleton_method(cUInt32, "lt", num_uint32_s_lt, 2);
    rb_define_singleton_method(cUInt32, "le", num_uint32_s_le, 2);

    //rb_define_method(cUInt32, "-@",    num_uint32_minus, 0);
    rb_define_method(cUInt32, "abs",   num_uint32_self, 0);
    rb_define_method(cUInt32, "floor", num_uint32_self, 0);
    rb_define_method(cUInt32, "round", num_uint32_self, 0);
    rb_define_method(cUInt32, "ceil",  num_uint32_self, 0);

    //rb_define_method(cUInt32, "sum", num_uint32_sum, -1);
    //rb_define_method(cUInt32, "accum", num_uint32_accum, -1);
    rb_define_method(cUInt32, "min", num_uint32_min, -1);
    rb_define_method(cUInt32, "max", num_uint32_max, -1);

    rb_define_method(cUInt32, "seq", num_uint32_seq, -1);
    rb_define_method(cUInt32, "rand", num_uint32_rand, -1);

    rb_define_method(cUInt32, "sort", num_uint32_sort, -1);
    rb_define_method(cUInt32, "sort!", num_uint32_sort_bang, -1);
    rb_define_method(cUInt32, "sort_index", num_uint32_sort_index, -1);
    rb_define_method(cUInt32, "median", num_uint32_median, -1);

    rb_define_method(cUInt32, "_cast_to", num_uint32__cast_to, 1);

    rb_define_method(cUInt32, "debug_print", num_uint32_debug_print, 0);
    rb_define_method(cUInt16, "debug_print", num_uint16_debug_print, 0);
    rb_define_method(cUInt8,  "debug_print", num_uint8_debug_print, 0);
    rb_define_method(cUInt24, "debug_print", num_uint24_debug_print, 0);

    rb_define_method(cUInt32, "inspect", num_uint32_inspect, 0);
    rb_define_method(cUInt32, "format", num_uint32_format, -1);
    rb_define_method(cUInt32, "format_to_a", num_uint32_format_to_a, -1);
    rb_define_method(cUInt24, "inspect", num_uint24_inspect, 0);
    rb_define_method(cUInt24, "format", num_uint24_format, -1);
    rb_define_method(cUInt24, "format_to_a", num_uint24_format_to_a, -1);
    rb_define_method(cUInt16, "inspect", num_uint16_inspect, 0);
    rb_define_method(cUInt16, "format", num_uint16_format, -1);
    rb_define_method(cUInt16, "format_to_a", num_uint16_format_to_a, -1);
    rb_define_method(cUInt8,  "inspect", num_uint8_inspect, 0);
    rb_define_method(cUInt8,  "format", num_uint8_format, -1);
    rb_define_method(cUInt8,  "format_to_a", num_uint8_format_to_a, -1);

    rb_define_method(cUInt32, "to_a", num_cast_uint32_to_array, 0);

    rb_define_method(cUInt32, "extract", num_uint32_extract, 0);

    hCast = rb_hash_new();
    rb_define_const(cUInt32, "UPCAST", hCast);
    rb_hash_aset(hCast, cUInt32,     cUInt32);
    rb_hash_aset(hCast, cUInt24,     cUInt32);
    rb_hash_aset(hCast, cUInt16,     cUInt32);
    rb_hash_aset(hCast, cUInt8,      cUInt32);
    rb_hash_aset(hCast, cInt32,      cInt32);
    rb_hash_aset(hCast, cInt24,      cInt32);
    rb_hash_aset(hCast, cInt16,      cInt32);
    rb_hash_aset(hCast, cInt8,       cInt32);
    rb_hash_aset(hCast, cDComplex,   cDComplex);
    rb_hash_aset(hCast, rb_cFixnum,  cUInt32);
    rb_hash_aset(hCast, rb_cBignum,  cUInt32);
    rb_hash_aset(hCast, rb_cInteger, cUInt32);
    rb_hash_aset(hCast, rb_cFloat,   cDFloat);
    rb_hash_aset(hCast, rb_cArray,   cUInt32);
}
