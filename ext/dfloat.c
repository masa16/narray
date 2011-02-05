/*
  dfloat.c
  Numerical Array Extention for Ruby
    (C) Copyright 1999-2007 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#include <ruby.h>
#include <math.h>
//#include <version.h>
#include "narray.h"
//#include "narray_local.h"
#include "bytedata.h"

#include "template.h"
#include "template_int.h"
#include "template_data.h"
#include "template_sort.h"

#include "SFMT.h"

EXTERN double round(double);
EXTERN double log2(double);
EXTERN double exp2(double);
EXTERN double exp10(double);

extern VALUE mNMathDFloat;

#define DFLOAT2VALUE(p)  rb_float_new(*(double*)(p))
#define VALUE2DFLOAT(p)  NUM2DBL(*(VALUE*)(p))

static VALUE
num_dfloat_extract( VALUE self )
{
    char *ptr;
    narray_t *na;
    GetNArray(self,na);

    if (na->ndim==0) {
        ptr = na_data_pointer_for_read(self);
	return rb_float_new(*(double*)(ptr + na->offset));
    }
    return self;
}

FILL_DATA_WITH_NUM( dfloat, double, cDFloat, y=NUM2DBL(x) )

CAST_NUM_TO_DATA(   dfloat, double, cDFloat, y=NUM2DBL(x) )
CAST_DATA_TO_ARRAY( dfloat, double, cDFloat, y=rb_float_new(x) )
CAST_ARRAY_TO_DATA( dfloat, double, cDFloat, z=NUM2DBL(x), z=y )

CAST_INT_TO_DATA( int32,  int32_t,  dfloat, double, cDFloat, y=x )
CAST_INT_TO_DATA( uint32, u_int32_t, dfloat, double, cDFloat, y=x )
CAST_INT_TO_DATA( int64,  int64_t,  dfloat, double, cDFloat, y=x )
CAST_INT_TO_DATA( uint64, u_int64_t, dfloat, double, cDFloat, y=x )

static VALUE
num_dfloat_s__cast(VALUE dfloat, VALUE obj)
{
    if (CLASS_OF(obj)==cDFloat) {
	return obj;
    } else if (rb_obj_is_kind_of(obj,cUInt32)) {
	return num_cast_uint32_to_dfloat(obj);
    } else if (rb_obj_is_kind_of(obj,cInt32)) {
	return num_cast_int32_to_dfloat(obj);
    } else if (rb_obj_is_kind_of(obj,cUInt64)) {
	return num_cast_uint64_to_dfloat(obj);
    } else if (rb_obj_is_kind_of(obj,cInt64)) {
	return num_cast_int64_to_dfloat(obj);
    } else if (TYPE(obj)==T_ARRAY) {
	return num_cast_array_to_dfloat(obj);
    } else if ( TYPE(obj)==T_FLOAT || FIXNUM_P(obj) || TYPE(obj)==T_BIGNUM ) {
	return num_cast_numeric_to_dfloat( obj );
    }
    return Qnil;
}

static VALUE
num_dfloat__cast_to( VALUE value, VALUE type )
{
    return Qnil;
}

static void
dfloat_print_loop( char *ptr, size_t pos, VALUE opt )
{
    printf("%g", *(double*)( ptr + pos ));
}

VALUE
num_dfloat_debug_print( VALUE ary )
{
    ndfunc_debug_print( ary, dfloat_print_loop, Qnil );
    return Qnil;
}

static VALUE
format_dfloat( VALUE fmt, void *x )
{
    if (NIL_P(fmt)) {
	char s[32];
	int n;
	n = sprintf(s,"%g",*(double*)x);
	return rb_str_new(s,n);
    }
    return rb_funcall( fmt, '%', 1, rb_float_new(*(double*)x) );
}

INSPECT_FUNC(dfloat)
FORMAT_FUNC(dfloat)

BINARY_FUNC(dfloat,cDFloat,double,add,x+=y)
BINARY_FUNC(dfloat,cDFloat,double,sub,x-=y)
BINARY_FUNC(dfloat,cDFloat,double,mul,x*=y)
BINARY_FUNC(dfloat,cDFloat,double,div,x/=y)
BINARY_FUNC(dfloat,cDFloat,double,mod,x=fmod(x,y))
BINARY_FUNC(dfloat,cDFloat,double,pow_dfloat,x=pow(x,y))
// divmod

POW_DATA_INT(double)
BINARY_DI_FUNC(dfloat,cDFloat,double,pow_int32,{x=pow_double_int(x,y);})
static VALUE
num_dfloat_s_pow(VALUE mod, VALUE self, VALUE other)
{
    if (FIXNUM_P(other) || rb_obj_is_kind_of(other,cInt32)) {
	return num_dfloat_s_pow_int32(mod,self,other);
    } else {
        return num_dfloat_s_pow_dfloat(mod,self,other);
    }
}

UNARY_FUNC(dfloat,cDFloat,double,abs,    x=fabs(x))
UNARY_FUNC(dfloat,cDFloat,double,minus,  x=-x)
UNARY_FUNC(dfloat,cDFloat,double,inverse,x=1.0/x)
UNARY_FUNC(dfloat,cDFloat,double,floor,  x=floor(x))
UNARY_FUNC(dfloat,cDFloat,double,round,  x=round(x))
UNARY_FUNC(dfloat,cDFloat,double,ceil,   x=ceil(x))

CMP_BINARY_FUNC(dfloat,cDFloat,double,eq,x==y)
CMP_BINARY_FUNC(dfloat,cDFloat,double,ne,x!=y)
CMP_BINARY_FUNC(dfloat,cDFloat,double,gt,x>y)
CMP_BINARY_FUNC(dfloat,cDFloat,double,ge,x>=y)
CMP_BINARY_FUNC(dfloat,cDFloat,double,lt,x<y)
CMP_BINARY_FUNC(dfloat,cDFloat,double,le,x<=y)

CMP_UNARY_FUNC(dfloat,cDFloat,double,isnan,isnan(x))
CMP_UNARY_FUNC(dfloat,cDFloat,double,isinf,isinf(x))
CMP_UNARY_FUNC(dfloat,cDFloat,double,isfinite,isfinite(x))

ACCUM_UNARY_FUNC(dfloat,cDFloat,double,sum,
		 {func->args[1].init = INT2FIX(0);},
 		 (!isnan(x) && x!=0), {x+=y;} )
ACCUM_UNARY_FUNC(dfloat,cDFloat,double,accum,
		 {func->args[1].init = INT2FIX(0); NDF_SET(func,NDF_KEEP_DIM);},
 		 (!isnan(x) && x!=0), {x+=y;} )

ACCUM_UNARY_FUNC(dfloat,cDFloat,double,min,
		 {func->args[1].init = num_dfloat_new_dim0( 0.0/0.0 );},
		 (!isnan(x) && (isnan(y) || y>x)), {} )
ACCUM_UNARY_FUNC(dfloat,cDFloat,double,max,
		 {func->args[1].init = num_dfloat_new_dim0( 0.0/0.0 );},
		 (!isnan(x) && (isnan(y) || y<x)), {} )

#define cmp(a,b)						      \
    ( isnan(*(double*)(a)) ? (isnan(*(double*)(b)) ? 0 : 1) :	      \
      (isnan(*(double*)(b)) ? -1 :				      \
       ( (*(double*)(a)==*(double*)(b)) ? 0 :			      \
	 (*(double*)(a) > *(double*)(b)) ? 1 : -1) ))
#define cmpgt(a,b)						\
    ( (isnan(*(double*)(a)) && !isnan(*(double*)(b))) ||	\
      (*(double*)(a) > *(double*)(b)) )

QSORT_FUNC(dfloat,double)
SORT_FUNC(dfloat,cDFloat,double)

#undef cmp
#undef cmpgt
#define cmp(a,b)							\
    ( isnan(**(double**)(a)) ? (isnan(**(double**)(b)) ? 0 : 1) :	\
      (isnan(**(double**)(b)) ? -1 :					\
       ( (**(double**)(a)==**(double**)(b)) ? 0 :			\
	 (**(double**)(a) > **(double**)(b)) ? 1 : -1) ))
#define cmpgt(a,b)						\
    ( (isnan(**(double**)(a)) && !isnan(**(double**)(b))) ||	\
      (**(double**)(a) > **(double**)(b)) )

QSORT_FUNC(dfloat_index,double*)
SORT_INDEX_FUNC(dfloat,cDFloat,double)

MEDIAN_FUNC(dfloat,cDFloat,double)

// mean
// sort
// median
// Math
// histogram

UNARY_S_FUNC(dfloat,cDFloat,double,sqrt,x=sqrt(x))
UNARY_S_FUNC(dfloat,cDFloat,double,cbrt,x=cbrt(x))
UNARY_S_FUNC(dfloat,cDFloat,double,log,x=log(x))
UNARY_S_FUNC(dfloat,cDFloat,double,log2,x=log2(x))
UNARY_S_FUNC(dfloat,cDFloat,double,log10,x=log10(x))
UNARY_S_FUNC(dfloat,cDFloat,double,exp,x=exp(x))
UNARY_S_FUNC(dfloat,cDFloat,double,exp2,x=exp2(x))
UNARY_S_FUNC(dfloat,cDFloat,double,exp10,x=exp10(x))
UNARY_S_FUNC(dfloat,cDFloat,double,sin,x=sin(x))
UNARY_S_FUNC(dfloat,cDFloat,double,cos,x=cos(x))
UNARY_S_FUNC(dfloat,cDFloat,double,tan,x=tan(x))
UNARY_S_FUNC(dfloat,cDFloat,double,asin,x=asin(x))
UNARY_S_FUNC(dfloat,cDFloat,double,acos,x=acos(x))
UNARY_S_FUNC(dfloat,cDFloat,double,atan,x=atan(x))
UNARY_S_FUNC(dfloat,cDFloat,double,sinh,x=sinh(x))
UNARY_S_FUNC(dfloat,cDFloat,double,cosh,x=cosh(x))
UNARY_S_FUNC(dfloat,cDFloat,double,tanh,x=tanh(x))
UNARY_S_FUNC(dfloat,cDFloat,double,asinh,x=asinh(x))
UNARY_S_FUNC(dfloat,cDFloat,double,acosh,x=acosh(x))
UNARY_S_FUNC(dfloat,cDFloat,double,atanh,x=atanh(x))
BINARY_FUNC(dfloat,cDFloat,double,atan2,x=atan2(x,y))
BINARY_FUNC(dfloat,cDFloat,double,hypot,x=hypot(x,y))

UNARY_S_FUNC(dfloat,cDFloat,double,erf,x=erf(x))
UNARY_S_FUNC(dfloat,cDFloat,double,erfc,x=erfc(x))
BINARY_FUNC(dfloat,cDFloat,double,ldexp,x=ldexp(x,y))
//UNARY_S_FUNC(dfloat,cDFloat,double,frexp,x=frexp(x))

SEQ_DATA_FUNC( dfloat, cDFloat, double, {y=x;} )

RAND_DATA_FUNC( dfloat, cDFloat, double, rand, {y=to_res53(gen_rand64());} )
RAND_NORM_DATA_FUNC( dfloat, cDFloat, double, {rand_norm(a);} )


static void
dfloat_poly_loop( na_iterator_t *const itr, VALUE info )
{
    size_t  i;
    char   *p2;
    ssize_t s2;
    ssize_t *idx2;
    double  x, y;

    x = *(double*)(itr[0].ptr+itr[0].pos);
    INIT_PTR( itr, 1, p2, s2, idx2 );
    i = itr[1].n - 1;
    if (idx2) {
	y = *(double*)(p2+idx2[i]);
	for (; i; ) {
	    y *= x;
	    y += *(double*)(p2+idx2[--i]);
	}
    } else {
	y = *(double*)(p2+s2*(i));
	for (; i; ) {
	    y *= x;
	    y += *(double*)(p2+s2*(--i));
	}
    }
    *(double*)(itr[2].ptr+itr[2].pos) = y;
}


static VALUE
num_dfloat_s_poly( VALUE mod, VALUE a1, VALUE a2 )
{
    ndfunc_t *func;
    VALUE v;

    func = ndfunc_alloc( dfloat_poly_loop, NO_LOOP,
			 2, 1, cDFloat, cDFloat, cDFloat );
    func->args[1].dim = 1;
    v = ndfunc_execute( func, 2, a1, a2 );
    ndfunc_free(func);
    return num_dfloat_extract(v);
}

void
Init_dfloat()
{
    volatile VALUE hCast;
    //rb_require("stringio");

    rb_define_const(cDFloat, ELEMENT_BIT_SIZE,  INT2FIX(sizeof(double)*8));
    rb_define_const(cDFloat, ELEMENT_BYTE_SIZE, INT2FIX(sizeof(double)));
    rb_define_const(cDFloat, CONTIGUOUS_STRIDE, INT2FIX(sizeof(double)));

    //rb_define_singleton_method(cDFloat, "bit_size", num_dfloat_s_bit_size, 0);
    //rb_define_singleton_method(cDFloat, "byte_size", num_dfloat_s_byte_size, 0);
    //rb_define_singleton_method(cDFloat, "step_unit", num_dfloat_s_byte_size, 0);

    rb_define_singleton_method(cDFloat, "_cast", num_dfloat_s__cast, 1);
    rb_define_method(cDFloat, "store_array", num_dfloat_store_array, 1);

    //rb_define_method(cDFloat, "add!", num_dfloat_add_bang, 1);

    rb_define_singleton_method(cDFloat, "add", num_dfloat_s_add, 2);
    rb_define_singleton_method(cDFloat, "sub", num_dfloat_s_sub, 2);
    rb_define_singleton_method(cDFloat, "mul", num_dfloat_s_mul, 2);
    rb_define_singleton_method(cDFloat, "div", num_dfloat_s_div, 2);
    rb_define_singleton_method(cDFloat, "mod", num_dfloat_s_mod, 2);
    rb_define_singleton_method(cDFloat, "pow", num_dfloat_s_pow, 2);

    //rb_define_singleton_method(cDFloat, "dot_mm", num_dfloat_s_dot_mm, 2);

    rb_define_singleton_method(cDFloat, "eq", num_dfloat_s_eq, 2);
    rb_define_singleton_method(cDFloat, "ne", num_dfloat_s_ne, 2);
    rb_define_singleton_method(cDFloat, "gt", num_dfloat_s_gt, 2);
    rb_define_singleton_method(cDFloat, "ge", num_dfloat_s_ge, 2);
    rb_define_singleton_method(cDFloat, "lt", num_dfloat_s_lt, 2);
    rb_define_singleton_method(cDFloat, "le", num_dfloat_s_le, 2);

    rb_define_singleton_method(cDFloat, "poly", num_dfloat_s_poly, 2);

    rb_define_method(cDFloat, "nan?", num_dfloat_isnan, 0);
    rb_define_method(cDFloat, "inf?", num_dfloat_isinf, 0);
    rb_define_method(cDFloat, "finite?", num_dfloat_isfinite, 0);

    rb_define_method(cDFloat, "minus", num_dfloat_minus, 0);
    rb_define_method(cDFloat, "inverse", num_dfloat_inverse, 0);
    rb_define_method(cDFloat, "abs",   num_dfloat_abs, 0);
    rb_define_method(cDFloat, "floor", num_dfloat_floor, 0);
    rb_define_method(cDFloat, "round", num_dfloat_round, 0);
    rb_define_method(cDFloat, "ceil",  num_dfloat_ceil, 0);

    rb_define_method(cDFloat, "sum", num_dfloat_sum, -1);
    rb_define_method(cDFloat, "accum", num_dfloat_accum, -1);
    rb_define_method(cDFloat, "min", num_dfloat_min, -1);
    rb_define_method(cDFloat, "max", num_dfloat_max, -1);

    rb_define_method(cDFloat, "sort", num_dfloat_sort, -1);
    rb_define_method(cDFloat, "sort!", num_dfloat_sort_bang, -1);
    rb_define_method(cDFloat, "sort_index", num_dfloat_sort_index, -1);
    rb_define_method(cDFloat, "median", num_dfloat_median, -1);

    rb_define_method(cDFloat, "seq", num_dfloat_seq, -1);
    rb_define_method(cDFloat, "rand", num_dfloat_rand, 0);
    rb_define_method(cDFloat, "rand_norm", num_dfloat_rand_norm, 0);
    rb_define_method(cDFloat, "fill", num_dfloat_fill, 1);

    rb_define_method(cDFloat, "_cast_to", num_dfloat__cast_to, 1);
    rb_define_method(cDFloat, "debug_print", num_dfloat_debug_print, 0);

    rb_define_method(cDFloat, "inspect", num_dfloat_inspect, 0);
    rb_define_method(cDFloat, "format", num_dfloat_format, -1);
    rb_define_method(cDFloat, "format_to_a", num_dfloat_format_to_a, -1);

    rb_define_method(cDFloat, "to_a", num_cast_dfloat_to_array, 0);

    rb_define_method(cDFloat, "extract", num_dfloat_extract, 0);


    rb_define_singleton_method(mNMathDFloat, "sqrt", num_dfloat_s_sqrt, 1);
    rb_define_singleton_method(mNMathDFloat, "cbrt", num_dfloat_s_cbrt, 1);
    rb_define_singleton_method(mNMathDFloat, "log", num_dfloat_s_log, 1);
    rb_define_singleton_method(mNMathDFloat, "log2", num_dfloat_s_log2, 1);
    rb_define_singleton_method(mNMathDFloat, "log10", num_dfloat_s_log10, 1);
    rb_define_singleton_method(mNMathDFloat, "exp", num_dfloat_s_exp, 1);
    rb_define_singleton_method(mNMathDFloat, "exp2", num_dfloat_s_exp2, 1);
    rb_define_singleton_method(mNMathDFloat, "exp10", num_dfloat_s_exp10, 1);
    rb_define_singleton_method(mNMathDFloat, "sin", num_dfloat_s_sin, 1);
    rb_define_singleton_method(mNMathDFloat, "cos", num_dfloat_s_cos, 1);
    rb_define_singleton_method(mNMathDFloat, "tan", num_dfloat_s_tan, 1);
    rb_define_singleton_method(mNMathDFloat, "asin", num_dfloat_s_asin, 1);
    rb_define_singleton_method(mNMathDFloat, "acos", num_dfloat_s_acos, 1);
    rb_define_singleton_method(mNMathDFloat, "atan", num_dfloat_s_atan, 1);
    rb_define_singleton_method(mNMathDFloat, "sinh", num_dfloat_s_sinh, 1);
    rb_define_singleton_method(mNMathDFloat, "cosh", num_dfloat_s_cosh, 1);
    rb_define_singleton_method(mNMathDFloat, "tanh", num_dfloat_s_tanh, 1);
    rb_define_singleton_method(mNMathDFloat, "asinh", num_dfloat_s_asinh, 1);
    rb_define_singleton_method(mNMathDFloat, "acosh", num_dfloat_s_acosh, 1);
    rb_define_singleton_method(mNMathDFloat, "atanh", num_dfloat_s_atanh, 1);
    rb_define_singleton_method(mNMathDFloat, "atan2", num_dfloat_s_atan2, 2);
    rb_define_singleton_method(mNMathDFloat, "hypot", num_dfloat_s_hypot, 2);
    rb_define_singleton_method(mNMathDFloat, "erf", num_dfloat_s_erf, 1);
    rb_define_singleton_method(mNMathDFloat, "erfc", num_dfloat_s_erfc, 1);
    rb_define_singleton_method(mNMathDFloat, "ldexp", num_dfloat_s_ldexp, 2);


    hCast = rb_hash_new();
    rb_define_const(cDFloat, "UPCAST", hCast);
    rb_hash_aset(hCast, cInt64,      cDFloat);
    rb_hash_aset(hCast, cInt48,      cDFloat);
    rb_hash_aset(hCast, cInt32,      cDFloat);
    rb_hash_aset(hCast, cInt24,      cDFloat);
    rb_hash_aset(hCast, cInt16,      cDFloat);
    rb_hash_aset(hCast, cInt8,       cDFloat);
    rb_hash_aset(hCast, cUInt64,     cDFloat);
    rb_hash_aset(hCast, cUInt48,     cDFloat);
    rb_hash_aset(hCast, cUInt32,     cDFloat);
    rb_hash_aset(hCast, cUInt24,     cDFloat);
    rb_hash_aset(hCast, cUInt16,     cDFloat);
    rb_hash_aset(hCast, cUInt8,      cDFloat);
    rb_hash_aset(hCast, rb_cArray,   cDFloat);
    rb_hash_aset(hCast, rb_cFixnum,  cDFloat);
    rb_hash_aset(hCast, rb_cBignum,  cDFloat);
    rb_hash_aset(hCast, rb_cInteger, cDFloat);
    rb_hash_aset(hCast, rb_cFloat,   cDFloat);
    rb_hash_aset(hCast, cDComplex,   cDComplex);
    rb_hash_aset(hCast, cRObject,    cRObject);
}
