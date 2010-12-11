/*
  datatype.c
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

VALUE cDFloat, cDComplex;
VALUE cInt32, cInt24, cInt16, cInt8;
VALUE cUInt32, cUInt24, cUInt16, cUInt8;
VALUE cInt64, cInt48;
VALUE cUInt64, cUInt48;
VALUE cBit, cRObject;
VALUE cPointer;

VALUE cComplex;

ID id_add;
ID id_sub;
ID id_mul;
ID id_div;
ID id_mod;
ID id_pow;
ID id_bit_and;
ID id_bit_or;
ID id_bit_xor;
ID id_eq;
ID id_ne;
ID id_gt;
ID id_ge;
ID id_lt;
ID id_le;

static VALUE hFunc;


static VALUE
 num_type_s__cast(VALUE mod, VALUE value)
{
    rb_notimplement();
    return Qnil;
}

static VALUE
 num_type__cast_to(VALUE value, VALUE elem)
{
    rb_notimplement();
    return Qnil;
}

VALUE
num_type_s_cast(VALUE type, VALUE obj)
{
    VALUE r=Qnil;

    if (RTEST(rb_class_inherited_p(type,cNArray))) {
	r = rb_funcall( type, rb_intern("_cast"), 1, obj );
    }
    if (!RTEST(r))  {
	if (IsNArray(obj))
	    r = rb_funcall( obj, rb_intern("_cast_to"), 1, type );
	if (!RTEST(r))
	    rb_raise(num_eCastError, "cannot cast from %s to %s",
		     rb_class2name(CLASS_OF(obj)),
		     rb_class2name(type));
    }
    return r;
}


static VALUE
num_type_cast_to(VALUE obj, VALUE type)
{
    VALUE r=Qnil;

    if (IsNArray(obj))
	r = rb_funcall( obj, rb_intern("_cast_to"), 1, type );
    if (!RTEST(r))  {
	if (RTEST(rb_class_inherited_p(type,cNArray))) 
	    r = rb_funcall( type, rb_intern("_cast"), 1, obj );
	if (!RTEST(r))
	    rb_raise(num_eCastError, "cannot cast from %s to %s",
		     rb_class2name(CLASS_OF(obj)),
		     rb_class2name(type));
    }
    return r;
}


static VALUE
num_cast_array_to_narray( VALUE rary )
{
    VALUE v, tp, nary;
    ndfunc_t *func;
    size_t *shape;
    int nd;

    shape = na_mdarray_investigate(rary, &nd, &tp);
    nary = rb_narray_new(tp, nd, shape);
    xfree(shape);
    return rb_funcall( nary, rb_intern("store_array"), 1, rary );
}



VALUE
num_type_s_upcast(VALUE type1, VALUE type2)
{
    VALUE upcast_hash;
    VALUE result_type;

    //if (TYPE(type2)==T_CLASS) {
    //	if ( RTEST(rb_class_inherited_p(type2,rb_cInteger)) ) {
    //	    type2 = rb_cInteger;
    //	}
    //}
    if (type1==type2) return type1;
    upcast_hash = rb_const_get( type1, rb_intern("UPCAST") );
    result_type = rb_hash_aref( upcast_hash, type2 );
    if (NIL_P(result_type)) {
	if (TYPE(type2)==T_CLASS) {
	    if ( RTEST(rb_class_inherited_p(type2,cNArray)) ) {
		upcast_hash = rb_const_get( type2, rb_intern("UPCAST") );
		result_type = rb_hash_aref( upcast_hash, type1 );
	    }
	}
    }
    return result_type;
}


static VALUE
num_type_coerce(VALUE x, VALUE y)
{
    VALUE type;

    type = num_type_s_upcast( CLASS_OF(x), CLASS_OF(y) );
    //puts("pass1");
    y = num_type_s_cast(type,y);
    //puts("pass2");
    return rb_assoc_new( y , x );
}



VALUE
na_dispatch_operation( VALUE self, VALUE other, ID opid )
{
    VALUE type;
    type = num_type_s_upcast( CLASS_OF(self), CLASS_OF(other) );
    if ( rb_obj_is_kind_of( type, rb_cClass ) ) {
        if ( RTEST(rb_class_inherited_p( type, cNArray )) ) {
	    return rb_funcall( type, opid, 2, self, other );
	}
    }
    rb_raise(num_eCastError, "upcast failed for %s and %s",
	     rb_class2name(CLASS_OF(self)),rb_class2name(CLASS_OF(other)));
    return Qnil;
}

#define DISPATCH_FUNC(opname)				\
 VALUE na_##opname( VALUE self, VALUE other )		\
 {  na_dispatch_operation( self, other, id_##opname ); }\

DISPATCH_FUNC(add)
DISPATCH_FUNC(sub)
DISPATCH_FUNC(mul)
DISPATCH_FUNC(div)
DISPATCH_FUNC(mod)
//DISPATCH_FUNC(pow)
DISPATCH_FUNC(eq)
DISPATCH_FUNC(ne)
DISPATCH_FUNC(gt)
DISPATCH_FUNC(ge)
DISPATCH_FUNC(lt)
DISPATCH_FUNC(le)
DISPATCH_FUNC(bit_and)
DISPATCH_FUNC(bit_or)
DISPATCH_FUNC(bit_xor)


VALUE
na_pow( VALUE self, VALUE other )
{
    VALUE type;
    if (rb_obj_is_kind_of(other,cInt32)) {
        return rb_funcall( CLASS_OF(self), id_pow, 2, self, other );
    } else {
        type = num_type_s_upcast( CLASS_OF(self), CLASS_OF(other) );
	if ( rb_obj_is_kind_of( type, rb_cClass ) ) {
	    if ( RTEST(rb_class_inherited_p( type, cNArray )) ) {
	      return rb_funcall( type, id_pow, 2, self, other );
	    }
	}
    }
    rb_raise(num_eCastError, "upcast failed for %s and %s",
	     rb_class2name(CLASS_OF(self)),rb_class2name(CLASS_OF(other)));
    return Qnil;
}


static VALUE
num_type_byte_size( VALUE self )
{
    VALUE klass, velmsz;
    narray_t *na;
    size_t sz;

    GetNArray(self,na);
    velmsz = rb_const_get( CLASS_OF(self), rb_intern(ELEMENT_BYTE_SIZE) );
    sz = SIZE2NUM( NUM2SIZE(velmsz) * na->size );
    return sz;
}


static VALUE
num_type_s_byte_size( VALUE type )
{
    return rb_const_get( type, rb_intern(ELEMENT_BYTE_SIZE) );
}


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
	for (j=0; j<e; j++) {
	  b2[e-1-j] = b1[j];
	}
	memcpy(q2,b2,e);
    }
}


static VALUE
num_type_swap_byte( VALUE self )
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


static VALUE
num_type_to_network( VALUE self )
{
    if (TEST_NETWORK_ORDER(self)) {
        return self;
    }
    return rb_funcall( self, rb_intern("swap_byte"), 0);
}

static VALUE
num_type_to_vacs( VALUE self )
{
    if (TEST_VACS_ORDER(self)) {
        return self;
    }
    return rb_funcall( self, rb_intern("swap_byte"), 0);
}

static VALUE
num_type_to_host( VALUE self )
{
  if (TEST_HOST_ORDER(self)) {
    puts("pass1");
        return self;
    }
    puts("pass2");
    return rb_funcall( self, rb_intern("swap_byte"), 0);
}

static VALUE
num_type_to_swapped( VALUE self )
{
    if (TEST_BYTE_SWAPPED(self)) {
        return self;
    }
    return rb_funcall( self, rb_intern("swap_byte"), 0);
}


void Init_dfloat();
void Init_bit();
void Init_int32();
void Init_uint32();
void Init_int64();


void
Init_datatype()
{
    //cDataTypeClassMethod = rb_define_module_under(mNum, "DataTypeClassMethod");

    rb_define_const(cNArray, "UPCAST", rb_hash_new());

    //rb_define_singleton_method(cNArray, "bit_step", num_type_s_bit_step, 0);
    //rb_define_singleton_method(cNArray, "bit_size", num_type_s_bit_size, 0);
    //rb_define_singleton_method(cNArray, "byte_size", num_type_s_byte_size, 0);
    rb_define_singleton_method(cNArray, "cast", num_type_s_cast, 1);
    rb_define_singleton_method(cNArray, "_cast", num_type_s__cast, 1);
    rb_define_singleton_method(cNArray, "upcast", num_type_s_upcast, 1);
    rb_define_singleton_method(cNArray, "cast_type", num_type_s_upcast, 1);

    rb_define_singleton_method(cNArray, "byte_size", num_type_s_byte_size, 0);

    rb_define_method(cNArray, "cast_to", num_type_cast_to, 1);
    rb_define_method(cNArray, "_cast_to", num_type__cast_to, 1);
    rb_define_method(cNArray, "coerce", num_type_coerce, 1);

    rb_define_method(cNArray, "+",  na_add, 1);
    rb_define_method(cNArray, "-",  na_sub, 1);
    rb_define_method(cNArray, "*",  na_mul, 1);
    rb_define_method(cNArray, "/",  na_div, 1);
    rb_define_method(cNArray, "%",  na_mod, 1);
    rb_define_method(cNArray, "**", na_pow, 1);
    rb_define_method(cNArray, "&",  na_bit_and, 1);
    rb_define_method(cNArray, "|",  na_bit_or,  1);
    rb_define_method(cNArray, "^",  na_bit_xor, 1);
    rb_define_method(cNArray, "and",na_bit_and, 1);
    rb_define_method(cNArray, "or", na_bit_or,  1);
    rb_define_method(cNArray, "xor",na_bit_xor, 1);
    rb_define_method(cNArray, "eq", na_eq, 1);
    rb_define_method(cNArray, "ne", na_ne, 1);
    rb_define_method(cNArray, "gt", na_gt, 1);
    rb_define_method(cNArray, "ge", na_ge, 1);
    rb_define_method(cNArray, "lt", na_lt, 1);
    rb_define_method(cNArray, "le", na_le, 1);
    rb_define_method(cNArray, ">",  na_gt, 1);
    rb_define_method(cNArray, ">=", na_ge, 1);
    rb_define_method(cNArray, "<",  na_lt, 1);
    rb_define_method(cNArray, "<=", na_le, 1);

    rb_define_method(cNArray, "byte_size",  num_type_byte_size, 0);

    rb_define_method(cNArray, "swap_byte", num_type_swap_byte, 0);
#ifdef DYNAMIC_ENDIAN
#else
#ifdef WORDS_BIGENDIAN
#else // LITTLE_ENDIAN
    rb_define_alias(cNArray, "hton", "swap_byte");
    rb_define_alias(cNArray, "hton", "swap_byte");
    rb_define_alias(cNArray, "network_order?", "byte_swapped?");
    rb_define_alias(cNArray, "little_endian?", "host_order?");
    rb_define_alias(cNArray, "vacs_order?", "host_order?");
#endif
#endif
    rb_define_method(cNArray, "to_network", num_type_to_network, 0);
    rb_define_method(cNArray, "to_vacs", num_type_to_vacs, 0);
    rb_define_method(cNArray, "to_host", num_type_to_host, 0);

    id_add = rb_intern("add");
    id_sub = rb_intern("sub");
    id_mul = rb_intern("mul");
    id_div = rb_intern("div");
    id_mod = rb_intern("mod");
    id_pow = rb_intern("pow");
    id_bit_and = rb_intern("bit_and");
    id_bit_or  = rb_intern("bit_or");
    id_bit_xor = rb_intern("bit_xor");
    id_eq = rb_intern("eq");
    id_ne = rb_intern("ne");
    id_gt = rb_intern("gt");
    id_ge = rb_intern("ge");
    id_lt = rb_intern("lt");
    id_le = rb_intern("le");

    cDFloat = rb_define_class_under(mNum, "DFloat", cNArray);
    rb_define_const(mNum, "Float64", cDFloat);

    cDComplex = rb_define_class_under(mNum, "DComplex", cNArray);
    rb_define_const(mNum, "Complex128", cDComplex);

    cInt32 = rb_define_class_under(mNum, "Int32", cNArray);
    cInt24 = rb_define_class_under(mNum, "Int24", cInt32);
    cInt16 = rb_define_class_under(mNum, "Int16", cInt32);
    cInt8  = rb_define_class_under(mNum, "Int8",  cInt32);

    cUInt32 = rb_define_class_under(mNum, "UInt32", cInt32);
    cUInt24 = rb_define_class_under(mNum, "UInt24", cUInt32);
    cUInt16 = rb_define_class_under(mNum, "UInt16", cUInt32);
    cUInt8  = rb_define_class_under(mNum, "UInt8",  cUInt32);

    cInt64  = rb_define_class_under(mNum, "Int64", cNArray);
    cInt48  = rb_define_class_under(mNum, "Int48", cInt64);
    cUInt64 = rb_define_class_under(mNum, "UInt64", cInt64);
    cUInt48 = rb_define_class_under(mNum, "UInt48", cUInt64);

    cBit     = rb_define_class_under(mNum, "Bit",   cNArray);
    cRObject = rb_define_class_under(mNum, "RObject", cNArray);

    cPointer = rb_define_class_under(mNum, "Pointer", cNArray);

    rb_require("complex");
    cComplex = rb_const_get( rb_cObject, rb_intern("Complex") );

    Init_math();

    Init_dfloat();
    Init_bit();
    Init_int32();
    Init_uint32();
    Init_int64();
    Init_dcomplex();
    Init_robject();
    Init_nstruct();
    Init_pointer();
}
