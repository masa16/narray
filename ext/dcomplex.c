/*
  dcomplex.c
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
#include "template_int.h"
#include "template_data.h"

#define real_t double
#define complex_t dcomplex
#include "template_comp.h"
#include "SFMT.h"

static ID id_real;
static ID id_imag;
extern VALUE mNMathDComplex;

static VALUE
num_dcomplex_extract( VALUE self )
{
    char *ptr;
    dcomplex x;
    narray_t *na;
    GetNArray(self,na);

    if (na->ndim==0) {
        ptr = na_data_pointer_for_read(self);
	x = *(dcomplex*)(ptr + na->offset);
	return COMP2NUM(x);
    }
    return self;
}

static dcomplex
num2dcomplex( VALUE v )
{
    dcomplex x;
    REAL(x) = NUM2DBL( rb_funcall((v),id_real,0) );
    IMAG(x) = NUM2DBL( rb_funcall((v),id_imag,0) );
    return x;
}

FILL_DATA_WITH_NUM( dcomplex, dcomplex, cDComplex, y=num2dcomplex(x) )
CAST_NUM_TO_DATA( dcomplex, dcomplex, cDComplex, y=num2dcomplex(x) )
CAST_DATA_TO_ARRAY( dcomplex, dcomplex, cDComplex, y=COMP2NUM(x) )
CAST_ARRAY_TO_DATA( dcomplex, dcomplex, cDComplex,
		    {z=num2dcomplex(x);}, {COMP_SET(y,0,z);} )
CAST_INT_TO_DATA( int32,  int32_t,  dcomplex, dcomplex, cDComplex, {COMP_SET(x,0,y);})
CAST_INT_TO_DATA( uint32, u_int32_t, dcomplex, dcomplex, cDComplex, {COMP_SET(x,0,y);})
CAST_INT_TO_DATA( int64,  int64_t,  dcomplex, dcomplex, cDComplex, {COMP_SET(x,0,y);})
CAST_INT_TO_DATA( uint64, u_int64_t, dcomplex, dcomplex, cDComplex, {COMP_SET(x,0,y);})

static VALUE
num_dcomplex_s__cast(VALUE dcomplex, VALUE obj)
{
    if (CLASS_OF(obj)==cDComplex) {
	return obj;
    } else if (rb_obj_is_kind_of(obj,cUInt32)) {
	return num_cast_uint32_to_dcomplex(obj);
    } else if (rb_obj_is_kind_of(obj,cInt32)) {
	return num_cast_int32_to_dcomplex(obj);
    } else if (rb_obj_is_kind_of(obj,cUInt64)) {
	return num_cast_uint64_to_dcomplex(obj);
    } else if (rb_obj_is_kind_of(obj,cInt64)) {
	return num_cast_int64_to_dcomplex(obj);
    } else if (TYPE(obj)==T_ARRAY) {
	return num_cast_array_to_dcomplex(obj);
    } else if (TYPE(obj)==T_FLOAT || FIXNUM_P(obj) || TYPE(obj)==T_BIGNUM ||
	       rb_obj_is_kind_of(obj,cComplex)) {
	return num_cast_numeric_to_dcomplex( obj );
    }
    return Qnil;
}

static VALUE
num_dcomplex__cast_to( VALUE value, VALUE type )
{
    return Qnil;
}


static void
dcomplex_print_loop( char *ptr, size_t pos, VALUE opt )
{
    dcomplex x = *(dcomplex*)( ptr + pos );
    printf("%g%+gi", REAL(x), IMAG(x));
}

VALUE
num_dcomplex_debug_print( VALUE ary )
{
    ndfunc_debug_print( ary, dcomplex_print_loop, Qnil );
    return Qnil;
}


static VALUE
format_dcomplex( VALUE fmt, void *x )
{
    if (NIL_P(fmt)) {
	char s[48];
	int n;
	n = sprintf(s,"%g%+gi",((dcomplex*)x)->dat[0],((dcomplex*)x)->dat[1]);
	return rb_str_new(s,n);
    }
    return rb_funcall( fmt, '%', 1,
		       rb_assoc_new(rb_float_new(((dcomplex*)x)->dat[0]),
				    rb_float_new(((dcomplex*)x)->dat[1])) );
}

INSPECT_FUNC(dcomplex)
FORMAT_FUNC(dcomplex)

BINARY_FUNC(dcomplex,cDComplex,dcomplex,add,COMP_ADD(x,y,x))
BINARY_FUNC(dcomplex,cDComplex,dcomplex,sub,COMP_SUB(x,y,x))
BINARY_FUNC(dcomplex,cDComplex,dcomplex,mul,COMP_MUL(x,y,x))
BINARY_FUNC(dcomplex,cDComplex,dcomplex,div,COMP_DIV(x,y,x))

COMP_POW_INT(dcomplex)
BINARY_DI_FUNC(dcomplex,cDComplex,dcomplex,pow_int,{x=pow_dcomplex_int(x,y);})
BINARY_FUNC(dcomplex,cDComplex,dcomplex,pow_dcomplex, COMP_POW(x,y,x) )

static VALUE
num_dcomplex_s_pow(VALUE mod, VALUE self, VALUE other)
{
    if (FIXNUM_P(other) || rb_obj_is_kind_of(other,cInt32)) {
	return num_dcomplex_s_pow_int(mod,self,other);
    } else {
        return num_dcomplex_s_pow_dcomplex(mod,self,other);
    }
}

UNARY_FUNC(dcomplex,cDComplex,dcomplex,minus,COMP_MINUS(x,x))
UNARY_FUNC(dcomplex,cDComplex,dcomplex,conj,{IMAG(x)=-IMAG(x);})
UNARY_FUNC(dcomplex,cDComplex,dcomplex,mul_imag,COMP_MUL_IMAG(x,x))
UNARY_FUNC2(dcomplex,cDComplex,dcomplex,cDFloat,double,abs,{y=hypot(REAL(x),IMAG(x));})
UNARY_FUNC2(dcomplex,cDComplex,dcomplex,cDFloat,double,real,{y=REAL(x);})
UNARY_FUNC2(dcomplex,cDComplex,dcomplex,cDFloat,double,imag,{y=IMAG(x);})
UNARY_FUNC2(dcomplex,cDComplex,dcomplex,cDFloat,double,arg,{y=atan2(IMAG(x),REAL(x));})

SET_FUNC2( dcomplex, cDComplex, dcomplex, cDFloat, double, imag_set, {IMAG(x)=y;})
SET_FUNC2( dcomplex, cDComplex, dcomplex, cDFloat, double, real_set, {REAL(x)=y;})

UNARY_FUNC(dcomplex,cDComplex,dcomplex,inverse, COMP_INVERSE(x,x))

//UNARY_FUNC(dcomplex,cDComplex,dcomplex,floor,  x=floor(x))
//UNARY_FUNC(dcomplex,cDComplex,dcomplex,round,  x=round(x))
//UNARY_FUNC(dcomplex,cDComplex,dcomplex,ceil,   x=ceil(x))

CMP_BINARY_FUNC(dcomplex,cDComplex,dcomplex,eq,COMP_EQ(x,y))
CMP_BINARY_FUNC(dcomplex,cDComplex,dcomplex,ne,COMP_NE(x,y))
//CMP_BINARY_FUNC(dcomplex,cDComplex,dcomplex,gt,x>y)
//CMP_BINARY_FUNC(dcomplex,cDComplex,dcomplex,ge,x>=y)
//CMP_BINARY_FUNC(dcomplex,cDComplex,dcomplex,lt,x<y)
//CMP_BINARY_FUNC(dcomplex,cDComplex,dcomplex,le,x<=y)

CMP_UNARY_FUNC(dcomplex,cDComplex,dcomplex,isnan, COMP_ISNAN(x))
CMP_UNARY_FUNC(dcomplex,cDComplex,dcomplex,isinf, COMP_ISINF(x))
CMP_UNARY_FUNC(dcomplex,cDComplex,dcomplex,isfinite, COMP_ISFINITE(x))

ACCUM_UNARY_FUNC(dcomplex,cDComplex,dcomplex,sum,
                 {func->args[1].init = INT2FIX(0);},
                 (!COMP_ISNAN(x) && !(REAL(x)==0&&IMAG(x)==0)), COMP_ADD(x,y,x) )
ACCUM_UNARY_FUNC(dcomplex,cDComplex,dcomplex,accum,
		 {func->args[1].init = INT2FIX(0); NDF_SET(func,NDF_KEEP_DIM);},
                 (!COMP_ISNAN(x) && !(REAL(x)==0&&IMAG(x)==0)), COMP_ADD(x,y,x) )

//ACCUM_UNARY_FUNC(dcomplex,cDComplex,dcomplex,min,
//		 {func->args[1].init = num_dcomplex_new_dim0( 0.0/0.0 );},
//		 (!isnan(x) && (isnan(y) || y>x)), {} )
//ACCUM_UNARY_FUNC(dcomplex,cDComplex,dcomplex,max,
//		 {func->args[1].init = num_dcomplex_new_dim0( 0.0/0.0 );},
//		 (!isnan(x) && (isnan(y) || y<x)), {} )

UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,sqrt,COMP_SQRT(x,x))
//UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,cbrt,x=cbrt(x))
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,log,COMP_LOG(x,x))
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,log2,
	     {COMP_LOG(x,x);REAL(x)*=M_LOG2E;IMAG(x)*=M_LOG2E;})
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,log10,
	     {COMP_LOG(x,x);REAL(x)*=M_LOG10E;IMAG(x)*=M_LOG10E;})
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,exp,COMP_EXP(x,x))
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,exp2,COMP_EXP2(x,x))
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,exp10,COMP_EXP10(x,x))
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,sin,  COMP_SIN(x,x))
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,cos,  COMP_COS(x,x))
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,tan,  COMP_TAN(x,x))
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,asin, COMP_ASIN(x,x))
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,acos, COMP_ACOS(x,x))
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,atan, COMP_ATAN(x,x))
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,sinh, COMP_SINH(x,x))
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,cosh, COMP_COSH(x,x))
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,tanh, COMP_TANH(x,x))
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,asinh,COMP_ASINH(x,x))
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,acosh,COMP_ACOSH(x,x))
UNARY_S_FUNC(dcomplex,cDComplex,dcomplex,atanh,COMP_ATANH(x,x))
//BINARY_FUNC (dcomplex,cDComplex,dcomplex,atan2,x=atan2(x,y))
//BINARY_FUNC (dcomplex,cDComplex,dcomplex,hypot,x=hypot(x,y))

//UNARY_S_FUNC(dfloat,cDFloat,double,erf,x=erf(x))
//UNARY_S_FUNC(dfloat,cDFloat,double,erfc,x=erfc(x))
//BINARY_FUNC(dfloat,cDFloat,double,ldexp,x=ldexp(x,y))

SEQ_DATA_FUNC( dcomplex, cDComplex, dcomplex, COMP_SET(x,0,y) )

#define rand_comp(x) {REAL(x)=to_res53(gen_rand64());IMAG(x)=to_res53(gen_rand64());}
RAND_DATA_FUNC( dcomplex, cDComplex, dcomplex, rand, {rand_comp(y);} )
RAND_DATA_FUNC( dcomplex, cDComplex, dcomplex, rand_norm,
		{rand_norm((double*)&y);} )

static void
swap_byte_loop( na_iterator_t *const itr, VALUE info )
{
    size_t  i, s1, s2;
    char   *p1, *p2;
    char   *q1, *q2;
    char   *b1, *b2;
    size_t *idx1, *idx2;
    size_t  e;
    int     j;

    INIT_COUNTER( itr, i );
    INIT_PTR_ELM( itr, 0, p1, s1, idx1, e );
    INIT_PTR( itr, 1, p2, s2, idx2 );
    b1 = ALLOCA_N( char, e );
    b2 = ALLOCA_N( char, e );
    for (; i--; ) {
        if (idx1) {
	    q1 = p1 + *idx1;
	    idx1++;
        } else {
	    q1 = p1;
	    p1 += s1;
	}
        if (idx2) {
	    q2 = p2 + *idx2;
	    idx2++;
        } else {
	    q2 = p2;
	    p2 += s2;
	}
	memcpy(b1,q1,e);
	for (j=0; j<e/2; j++) {
	  b2[e/2-1-j] = b1[j];
	}
	for (; j<e; j++) {
	  b2[e+e/2-1-j] = b1[j];
	}
	memcpy(q2,b2,e);
    }
}


static VALUE
num_dcomplex_swap_byte( VALUE self )
{
    ndfunc_t *func;
    VALUE v;

    func = ndfunc_alloc( swap_byte_loop, NDF_HAS_LOOP|NDF_ACCEPT_SWAP,
			 1, 1, Qnil, CLASS_OF(self) );
    v = ndfunc_execute( func, 1, self );
    if (self!=v)
      na_copy_flags( self, v );
    REVERSE_BYTE_SWAPPED(v);
    ndfunc_free(func);
    return v;
}


static void
dcomplex_poly_loop( na_iterator_t *const itr, VALUE info )
{
    size_t  i;
    char   *p2;
    ssize_t s2;
    ssize_t *idx2;
    dcomplex a, x, y;

    x = *(dcomplex*)(itr[0].ptr+itr[0].pos);
    INIT_PTR( itr, 1, p2, s2, idx2 );
    i = itr[1].n - 1;
    if (idx2) {
	y = *(dcomplex*)(p2+idx2[i]);
	for (; i; ) {
	    COMP_MUL(x,y,y);
	    a = *(dcomplex*)(p2+idx2[--i]);
	    COMP_ADD(y,a,y);
	}
    } else {
	y = *(dcomplex*)(p2+s2*(i));
	for (; i; ) {
	    COMP_MUL(x,y,y);
	    a = *(dcomplex*)(p2+s2*(--i));
	    COMP_ADD(y,a,y);
	}
    }
    *(dcomplex*)(itr[2].ptr+itr[2].pos) = y;
}

static VALUE
num_dcomplex_s_poly( VALUE mod, VALUE a1, VALUE a2 )
{
    ndfunc_t *func;
    VALUE v;

    func = ndfunc_alloc( dcomplex_poly_loop, NO_LOOP,
			 2, 1, cDComplex, cDComplex, cDComplex );
    func->args[1].dim = 1;
    v = ndfunc_execute( func, 2, a1, a2 );
    ndfunc_free(func);
    return v;
}


void
Init_dcomplex()
{
    volatile VALUE hCast;

    rb_define_const(cDComplex, ELEMENT_BIT_SIZE,  INT2FIX(sizeof(dcomplex)*8));
    rb_define_const(cDComplex, ELEMENT_BYTE_SIZE, INT2FIX(sizeof(dcomplex)));
    rb_define_const(cDComplex, CONTIGUOUS_STRIDE, INT2FIX(sizeof(dcomplex)));

    rb_define_singleton_method(cDComplex, "_cast", num_dcomplex_s__cast, 1);

    rb_define_singleton_method(cDComplex, "add", num_dcomplex_s_add, 2);
    rb_define_singleton_method(cDComplex, "sub", num_dcomplex_s_sub, 2);
    rb_define_singleton_method(cDComplex, "mul", num_dcomplex_s_mul, 2);
    rb_define_singleton_method(cDComplex, "div", num_dcomplex_s_div, 2);
    //rb_define_singleton_method(cDComplex, "mod", num_dcomplex_s_mod, 2);
    rb_define_singleton_method(cDComplex, "pow", num_dcomplex_s_pow, 2);

    rb_define_singleton_method(cDComplex, "eq", num_dcomplex_s_eq, 2);
    rb_define_singleton_method(cDComplex, "ne", num_dcomplex_s_ne, 2);
    //rb_define_singleton_method(cDComplex, "gt", num_dcomplex_s_gt, 2);
    //rb_define_singleton_method(cDComplex, "ge", num_dcomplex_s_ge, 2);
    //rb_define_singleton_method(cDComplex, "lt", num_dcomplex_s_lt, 2);
    //rb_define_singleton_method(cDComplex, "le", num_dcomplex_s_le, 2);

    rb_define_singleton_method(cDComplex, "poly", num_dcomplex_s_poly, 2);

    rb_define_method(cDComplex, "nan?", num_dcomplex_isnan, 0);
    rb_define_method(cDComplex, "inf?", num_dcomplex_isinf, 0);
    rb_define_method(cDComplex, "finite?", num_dcomplex_isfinite, 0);

    rb_define_method(cDComplex, "minus",  num_dcomplex_minus, 0);
    rb_define_method(cDComplex, "inverse",num_dcomplex_inverse, 0);
    rb_define_method(cDComplex, "conj",   num_dcomplex_conj, 0);
    rb_define_method(cDComplex, "abs",    num_dcomplex_abs, 0);
    rb_define_method(cDComplex, "real",   num_dcomplex_real, 0);
    rb_define_method(cDComplex, "imag",   num_dcomplex_imag, 0);
    rb_define_method(cDComplex, "arg",    num_dcomplex_arg, 0);
    rb_define_alias(cDComplex, "angle", "arg");
    //rb_define_method(cDComplex, "floor", num_dcomplex_floor, 0);
    //rb_define_method(cDComplex, "round", num_dcomplex_round, 0);
    //rb_define_method(cDComplex, "ceil",  num_dcomplex_ceil, 0);
    rb_define_method(cDComplex, "real=",  num_dcomplex_real_set, 1);
    rb_define_method(cDComplex, "imag=",  num_dcomplex_imag_set, 1);
    rb_define_method(cDComplex, "im",	  num_dcomplex_mul_imag, 0);

    rb_define_method(cDComplex, "sum", num_dcomplex_sum, -1);
    rb_define_method(cDComplex, "accum", num_dcomplex_accum, -1);
    //rb_define_method(cDComplex, "min", num_dcomplex_min, -1);
    //rb_define_method(cDComplex, "max", num_dcomplex_max, -1);

    rb_define_method(cDComplex, "seq", num_dcomplex_seq, -1);
    rb_define_method(cDComplex, "rand", num_dcomplex_rand, 0);
    rb_define_method(cDComplex, "rand_norm", num_dcomplex_rand_norm, 0);

    rb_define_method(cDComplex, "_cast_to", num_dcomplex__cast_to, 1);
    rb_define_method(cDComplex, "fill", num_dcomplex_fill, 1);
    rb_define_method(cDComplex, "debug_print", num_dcomplex_debug_print, 0);
    rb_define_method(cDComplex, "inspect", num_dcomplex_inspect, 0);
    rb_define_method(cDComplex, "format", num_dcomplex_format, -1);
    rb_define_method(cDComplex, "format_to_a", num_dcomplex_format_to_a, -1);

    rb_define_method(cDComplex, "to_a", num_cast_dcomplex_to_array, 0);

    rb_define_method(cDComplex, "extract", num_dcomplex_extract, 0);

    rb_define_method(cDComplex, "swap_byte", num_dcomplex_swap_byte, 0);


    rb_define_singleton_method(mNMathDComplex, "sqrt", num_dcomplex_s_sqrt, 1);
//  rb_define_singleton_method(mNMathDComplex, "cbrt", num_dcomplex_s_cbrt, 1);
    rb_define_singleton_method(mNMathDComplex, "log", num_dcomplex_s_log, 1);
    rb_define_singleton_method(mNMathDComplex, "log2", num_dcomplex_s_log2, 1);
    rb_define_singleton_method(mNMathDComplex, "log10", num_dcomplex_s_log10, 1);
    rb_define_singleton_method(mNMathDComplex, "exp", num_dcomplex_s_exp, 1);
    rb_define_singleton_method(mNMathDComplex, "exp2", num_dcomplex_s_exp2, 1);
    rb_define_singleton_method(mNMathDComplex, "exp10", num_dcomplex_s_exp10, 1);
    rb_define_singleton_method(mNMathDComplex, "sin", num_dcomplex_s_sin, 1);
    rb_define_singleton_method(mNMathDComplex, "cos", num_dcomplex_s_cos, 1);
    rb_define_singleton_method(mNMathDComplex, "tan", num_dcomplex_s_tan, 1);
    rb_define_singleton_method(mNMathDComplex, "asin", num_dcomplex_s_asin, 1);
    rb_define_singleton_method(mNMathDComplex, "acos", num_dcomplex_s_acos, 1);
    rb_define_singleton_method(mNMathDComplex, "atan", num_dcomplex_s_atan, 1);
    rb_define_singleton_method(mNMathDComplex, "sinh", num_dcomplex_s_sinh, 1);
    rb_define_singleton_method(mNMathDComplex, "cosh", num_dcomplex_s_cosh, 1);
    rb_define_singleton_method(mNMathDComplex, "tanh", num_dcomplex_s_tanh, 1);
    rb_define_singleton_method(mNMathDComplex, "asinh", num_dcomplex_s_asinh, 1);
    rb_define_singleton_method(mNMathDComplex, "acosh", num_dcomplex_s_acosh, 1);
    rb_define_singleton_method(mNMathDComplex, "atanh", num_dcomplex_s_atanh, 1);
//  rb_define_singleton_method(mNMathDComplex, "atan2", num_dcomplex_s_atan2, 2);
//  rb_define_singleton_method(mNMathDComplex, "erf", num_dcomplex_s_erf, 1);
//  rb_define_singleton_method(mNMathDComplex, "erfc", num_dcomplex_s_erfc, 1);
//  rb_define_singleton_method(mNMathDComplex, "ldexp", num_dcomplex_s_ldexp, 2);

    hCast = rb_hash_new();
    rb_define_const(cDComplex, "UPCAST", hCast);
    rb_hash_aset(hCast, cDComplex,   cDComplex);
    rb_hash_aset(hCast, cDFloat,     cDComplex);
    rb_hash_aset(hCast, cInt32,      cDComplex);
    rb_hash_aset(hCast, cInt24,      cDComplex);
    rb_hash_aset(hCast, cInt16,      cDComplex);
    rb_hash_aset(hCast, cInt8,       cDComplex);
    rb_hash_aset(hCast, cInt64,      cDComplex);
    rb_hash_aset(hCast, cInt48,      cDComplex);
    rb_hash_aset(hCast, cUInt32,     cDComplex);
    rb_hash_aset(hCast, cUInt24,     cDComplex);
    rb_hash_aset(hCast, cUInt16,     cDComplex);
    rb_hash_aset(hCast, cUInt8,      cDComplex);
    rb_hash_aset(hCast, cUInt64,     cDComplex);
    rb_hash_aset(hCast, cUInt48,     cDComplex);
    rb_hash_aset(hCast, rb_cArray,   cDComplex);
    rb_hash_aset(hCast, rb_cFixnum,  cDComplex);
    rb_hash_aset(hCast, rb_cBignum,  cDComplex);
    rb_hash_aset(hCast, rb_cInteger, cDComplex);
    rb_hash_aset(hCast, rb_cFloat,   cDComplex);
    rb_hash_aset(hCast, cComplex,    cDComplex);

    id_real = rb_intern("real");
    id_imag = rb_intern("imag");
}
