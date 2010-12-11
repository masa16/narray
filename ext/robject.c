/*
  robject.c
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
#include "template_comp.h"


static void
robject_mark_loop( na_iterator_t *const itr, VALUE id_method )
{
    size_t  i;
    char   *p1;
    ssize_t s1;
    ssize_t *idx1;
    volatile VALUE x;

    INIT_COUNTER( itr, i );
    INIT_PTR( itr, 0, p1, s1, idx1 );
    if (idx1) {
	for (; i--; ) {
	    x = *(VALUE*)(p1 + *idx1);
	    idx1++;
	    rb_gc_mark( x );
	}
    } else {
	for (; i--; ) {
	    x = *(VALUE*)(p1);
	    p1 += s1;
	    rb_gc_mark( x );
	}
    }
}

void
num_robject_mark( VALUE self )
{
    ndfunc_t *func;
    VALUE v;

    func = ndfunc_alloc( robject_mark_loop, HAS_LOOP, 1, 0, Qnil );
    ndfunc_execute( func, 1, self );
    ndfunc_free(func);
}




static VALUE
num_robject_extract( VALUE self )
{
    char *ptr;
    narray_t *na;
    GetNArray(self,na);

    if (na->ndim==0) {
        ptr = na_data_pointer_for_read(self);
	return *(VALUE*)ptr;
    }
    return self;
}

FILL_DATA_WITH_NUM( robject, VALUE, cRObject, y=x )
CAST_NUM_TO_DATA( robject, VALUE, cRObject, y=x )
CAST_DATA_TO_ARRAY( robject, VALUE, cRObject, y=x )
CAST_ARRAY_TO_DATA( robject, VALUE, cRObject, z=x, z=rb_float_new(y) )

//CAST_DATA_TO_VALUE( robject, VALUE, cRObject, array, rb_cArray, y=*(VALUE*)(x) )
CAST_DATA_TO_VALUE( dfloat, double, cDFloat, robject, cRObject, y=rb_float_new(x) )
CAST_DATA_TO_VALUE( dcomplex, dcomplex, cDComplex, robject, cRObject, y=COMP2NUM(x) )

CAST_INT_TO_DATA( int32,  int32_t,  robject, VALUE, cRObject, y=INT2NUM(x) )
CAST_INT_TO_DATA( uint32, u_int32_t, robject, VALUE, cRObject, y=UINT2NUM(x) )
CAST_INT_TO_DATA( int64,  int64_t,  robject, VALUE, cRObject, y=LONG2NUM(x) )
CAST_INT_TO_DATA( uint64, u_int64_t, robject, VALUE, cRObject, y=LONG2NUM(x) )

static VALUE
num_robject_s__cast(VALUE robject, VALUE obj)
{
    if (CLASS_OF(obj)==cRObject) {
	return obj;
    } else if (rb_obj_is_kind_of(obj,cDFloat)) {
	return num_cast_dfloat_to_robject(obj);
    } else if (rb_obj_is_kind_of(obj,cDComplex)) {
	return num_cast_dcomplex_to_robject(obj);
    } else if (rb_obj_is_kind_of(obj,cUInt32)) {
	return num_cast_uint32_to_robject(obj);
    } else if (rb_obj_is_kind_of(obj,cInt32)) {
	return num_cast_int32_to_robject(obj);
    } else if (rb_obj_is_kind_of(obj,cUInt64)) {
	return num_cast_uint64_to_robject(obj);
    } else if (rb_obj_is_kind_of(obj,cInt64)) {
	return num_cast_int64_to_robject(obj);
    } else if (TYPE(obj)==T_ARRAY) {
	return num_cast_array_to_robject(obj);
    } else if ( TYPE(obj)==T_FLOAT || FIXNUM_P(obj) || TYPE(obj)==T_BIGNUM ) {
	return num_cast_numeric_to_robject( obj );
    }
    return Qnil;
}

static VALUE
num_robject__cast_to( VALUE value, VALUE type )
{
    return Qnil;
}

static void
robject_print_loop( char *ptr, size_t pos, VALUE opt )
{
    VALUE v, s;
    v = *(VALUE*)(ptr + pos);
    s = rb_obj_as_string(rb_inspect(v));
    printf("%s", StringValueCStr(s));
}

VALUE
num_robject_debug_print( VALUE ary )
{
    ndfunc_debug_print( ary, robject_print_loop, Qnil );
    return Qnil;
}


static void
robject_unary_loop( na_iterator_t *const itr, VALUE id_method )
{
    size_t  i;
    char   *p1, *p2;
    ssize_t s1, s2;
    ssize_t *idx1, *idx2;
    volatile VALUE x;

    INIT_COUNTER( itr, i );
    INIT_PTR( itr, 0, p1, s1, idx1 );
    INIT_PTR( itr, 1, p2, s2, idx2 );
    for (; i--; ) {
	LOAD_DATA_STEP( p1, s1, idx1, VALUE, x );
	//printf("i=%d x=%x, id=%x\n",i,x,id_method);
	x = rb_funcall( x, id_method, 0 );
	//x = rb_funcall( x, rb_intern("abs"), 0 );
	STORE_DATA_STEP( p2, s2, idx2, VALUE, x );
    }
}

static VALUE
num_robject_method_missing( int argc, VALUE *argv, VALUE self )
{
    int i;
    VALUE v, id_method;
    ndfunc_t *func;

    id_method = SYM2ID(argv[0]);

    if (argc==1) {
	func = ndfunc_alloc( robject_unary_loop, HAS_LOOP, 1, 1, cRObject, cRObject );
	v = ndfunc_execute( func, 2, self, id_method );
	ndfunc_free( func );
	return v;
    }
    rb_raise( rb_eArgError, "wrong # of arbument");
    return Qnil;
}


static void
robject_binary_loop( na_iterator_t *const itr, VALUE id_method )
{
    size_t  i;
    char   *p1, *p2, *p3;
    ssize_t s1, s2, s3;
    ssize_t *idx1, *idx2, *idx3;
    volatile VALUE x;
    VALUE y;
    INIT_COUNTER( itr, i );
    INIT_PTR( itr, 0, p1, s1, idx1 );
    INIT_PTR( itr, 1, p2, s2, idx2 );
    INIT_PTR( itr, 2, p3, s3, idx3 );
    for (; i--; ) {
	LOAD_DATA_STEP( p1, s1, idx1, VALUE, x );
	LOAD_DATA_STEP( p2, s2, idx2, VALUE, y );
	x = rb_funcall( x, id_method, 1, y );
	STORE_DATA_STEP( p3, s3, idx3, VALUE, x );
    }
}

#define ROBJ_BINARY_FUNC( opname, methodid )				\
static VALUE								\
num_robject_s_##opname( VALUE mod, VALUE a1, VALUE a2 )		\
{									\
    ndfunc_t *func;							\
    VALUE v;								\
    func = ndfunc_alloc( robject_binary_loop, HAS_LOOP,			\
			 2, 1, cRObject, cRObject, cRObject );		\
    v = ndfunc_execute( func, 3, a1, a2, methodid );			\
    ndfunc_free(func);							\
    return v;								\
}

ROBJ_BINARY_FUNC(add,'+')
ROBJ_BINARY_FUNC(sub,'-')
ROBJ_BINARY_FUNC(mul,'*')
ROBJ_BINARY_FUNC(div,'/')
ROBJ_BINARY_FUNC(mod,'%')
ROBJ_BINARY_FUNC(pow,rb_intern("**"))


static void
robject_cmp_binary_loop( na_iterator_t *const itr, VALUE id_method )
{
    size_t  i;
    char   *p1, *p2;
    BIT_DIGIT *a3;
    size_t  p3;
    ssize_t s1, s2, s3;
    ssize_t *idx1, *idx2, *idx3;
    VALUE x, y;
    BIT_DIGIT b;
    INIT_COUNTER( itr, i );
    INIT_PTR( itr, 0, p1, s1, idx1 );
    INIT_PTR( itr, 1, p2, s2, idx2 );
    INIT_PTR_BIT( itr, 2, a3, p3, s3, idx3 );
    for (; i--; ) {
	LOAD_DATA_STEP( p1, s1, idx1, VALUE, x );
	LOAD_DATA_STEP( p2, s2, idx2, VALUE, y );
	x = rb_funcall( x, id_method, 1, y );
	b = (RTEST(x)) ? 1:0;
	STORE_BIT_STEP( a3, p3, s3, idx3, b );
    }
}

#define ROBJ_CMP_BINARY_FUNC( opname, methodid )		\
static VALUE							\
num_robject_s_##opname( VALUE mod, VALUE a1, VALUE a2 )		\
{								\
    ndfunc_t *func;						\
    VALUE v;							\
    func = ndfunc_alloc( robject_cmp_binary_loop, HAS_LOOP,	\
			 2, 1, cRObject, cRObject, cBit );	\
    v = ndfunc_execute( func, 3, a1, a2, methodid );		\
    ndfunc_free(func);						\
    return v;							\
}

ROBJ_CMP_BINARY_FUNC(eq,rb_intern("=="))
ROBJ_CMP_BINARY_FUNC(ne,rb_intern("!="))
ROBJ_CMP_BINARY_FUNC(ge,rb_intern(">="))
ROBJ_CMP_BINARY_FUNC(le,rb_intern("<="))
ROBJ_CMP_BINARY_FUNC(gt,'>')
ROBJ_CMP_BINARY_FUNC(lt,'<')


ACCUM_UNARY_FUNC(robject,cRObject,VALUE,sum,
		 {func->args[1].init = INT2FIX(0);},
 		 (1), x=rb_funcall(x,'+',1,y) )

/*
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
*/
/*
BINARY_FUNC(robject,cRObject,VALUE,func2,x=rb_funcall(:x+=y)

UNARY_FUNC(robject,cRObject,VALUE,abs,    x=fabs(x))

CMP_BINARY_FUNC(robject,cRObject,VALUE,eq,x==y)

ACCUM_UNARY_FUNC(robject,cRObject,VALUE,sum,
		 {func->args[1].init = INT2FIX(0);},
 		 (!isnan(x) && x!=0), {x+=y;} )
ACCUM_UNARY_FUNC(robject,cRObject,VALUE,accum,
		 {func->args[1].init = INT2FIX(0); NDF_SET(func,NDF_KEEP_DIM);},
 		 (!isnan(x) && x!=0), {x+=y;} )

ACCUM_UNARY_FUNC(robject,cRObject,VALUE,min,
		 {func->args[1].init = num_robject_new_dim0( 0.0/0.0 );},
		 (!isnan(x) && (isnan(y) || y>x)), {} )
ACCUM_UNARY_FUNC(robject,cRObject,VALUE,max,
		 {func->args[1].init = num_robject_new_dim0( 0.0/0.0 );},
		 (!isnan(x) && (isnan(y) || y<x)), {} )

*/

SEQ_DATA_FUNC( robject, cRObject, VALUE, y=rb_float_new(x) )

void
Init_robject()
{
    volatile VALUE hCast;

    rb_define_const(cRObject, ELEMENT_BIT_SIZE,  INT2FIX(sizeof(VALUE)*8));
    rb_define_const(cRObject, ELEMENT_BYTE_SIZE, INT2FIX(sizeof(VALUE)));
    rb_define_const(cRObject, CONTIGUOUS_STRIDE, INT2FIX(sizeof(VALUE)));

    rb_define_singleton_method(cRObject, "_cast", num_robject_s__cast, 1);

    rb_define_method(cRObject, "seq", num_robject_seq, -1);

    rb_define_method(cRObject, "_cast_to", num_robject__cast_to, 1);
    rb_define_method(cRObject, "fill", num_robject_fill, 1);
    rb_define_method(cRObject, "debug_print", num_robject_debug_print, 0);

    rb_define_method(cRObject, "to_a", num_cast_robject_to_array, 0);

    rb_define_method(cRObject, "extract", num_robject_extract, 0);

    rb_define_method(cRObject, "method_missing", num_robject_method_missing, -1);
    //rb_define_singleton_method(cRObject, "method_missing", num_robject_s_method_missing, -1);

    rb_define_method(cRObject, "sum", num_robject_sum, -1);

    rb_define_singleton_method(cRObject, "add", num_robject_s_add, 2);
    rb_define_singleton_method(cRObject, "sub", num_robject_s_sub, 2);
    rb_define_singleton_method(cRObject, "mul", num_robject_s_mul, 2);
    rb_define_singleton_method(cRObject, "div", num_robject_s_div, 2);
    rb_define_singleton_method(cRObject, "mod", num_robject_s_mod, 2);
    rb_define_singleton_method(cRObject, "pow", num_robject_s_pow, 2);
    rb_define_singleton_method(cRObject, "eq",  num_robject_s_eq, 2);
    rb_define_singleton_method(cRObject, "ne",  num_robject_s_ne, 2);
    rb_define_singleton_method(cRObject, "gt",  num_robject_s_gt, 2);
    rb_define_singleton_method(cRObject, "ge",  num_robject_s_ge, 2);
    rb_define_singleton_method(cRObject, "lt",  num_robject_s_lt, 2);
    rb_define_singleton_method(cRObject, "le",  num_robject_s_le, 2);

    hCast = rb_hash_new();
    rb_define_const(cRObject, "UPCAST", hCast);
    rb_hash_aset(hCast, cInt64,      cRObject);
    rb_hash_aset(hCast, cInt48,      cRObject);
    rb_hash_aset(hCast, cInt32,      cRObject);
    rb_hash_aset(hCast, cInt24,      cRObject);
    rb_hash_aset(hCast, cInt16,      cRObject);
    rb_hash_aset(hCast, cInt8,       cRObject);
    rb_hash_aset(hCast, cUInt64,     cRObject);
    rb_hash_aset(hCast, cUInt48,     cRObject);
    rb_hash_aset(hCast, cUInt32,     cRObject);
    rb_hash_aset(hCast, cUInt24,     cRObject);
    rb_hash_aset(hCast, cUInt16,     cRObject);
    rb_hash_aset(hCast, cUInt8,      cRObject);
    rb_hash_aset(hCast, rb_cArray,   cRObject);
    rb_hash_aset(hCast, rb_cFixnum,  cRObject);
    rb_hash_aset(hCast, rb_cBignum,  cRObject);
    rb_hash_aset(hCast, rb_cInteger, cRObject);
    rb_hash_aset(hCast, rb_cFloat,   cRObject);
    rb_hash_aset(hCast, cDFloat,     cRObject);
    rb_hash_aset(hCast, cDComplex,   cRObject);
}
