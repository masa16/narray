/*
  math.c
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2007 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#include <ruby.h>
#include "narray.h"

VALUE mNMath, mNMathDFloat, mNMathDComplex;
static ID id_send;

VALUE num_math_cast2(VALUE type1, VALUE type2)
{
    if ( RTEST(rb_class_inherited_p( type1, cNArray )) ){
	return num_type_s_upcast( type1, type2 );
    }
    if ( RTEST(rb_class_inherited_p( type2, cNArray )) ){
	return num_type_s_upcast( type2, type1 );
    }
    if ( RTEST(rb_class_inherited_p( type1, rb_cNumeric )) &&
	 RTEST(rb_class_inherited_p( type2, rb_cNumeric )) ){
	if ( RTEST(rb_class_inherited_p( type1, cComplex)) ||
	     RTEST(rb_class_inherited_p( type2, cComplex )) ){
	    return cComplex;
	}
	return rb_cFloat;
    }
    return type2;
}


VALUE na_array_type(VALUE);

VALUE num_mathcast(int argc, VALUE *argv)
{
    VALUE type, mathcast_hash, result_type;
    int i;
    type = rb_cFixnum;
    for (i=0; i<argc; i++) {
	type = num_math_cast2( type, na_array_type(argv[i]) );
	if (NIL_P(type)) {
	    rb_raise(rb_eTypeError,"%s is invalid for NMath",
		     rb_class2name(argv[i]));
	}
    }
    return type;
}


VALUE num_math_method_missing(int argc, VALUE *argv, VALUE mod)
{
    VALUE type, ans, typemod, hash;
    if (argc>1) {
	type = num_mathcast(argc-1,argv+1);

	hash = rb_const_get( mod, rb_intern("TYPE_MODULE") );
	typemod = rb_hash_aref( hash, type );
	if (NIL_P(typemod)) {
	    rb_raise(rb_eTypeError,"%s is invalid for NMath",
		     rb_class2name(type));
	}

	ans = rb_funcall2(typemod,id_send,argc,argv);

	if (!RTEST(rb_class_inherited_p(type,cNArray)) &&
	    IsNArray(ans) ) {
	    ans = rb_funcall(ans,rb_intern("extract"),0);
	}
	return ans;
    }
    rb_raise(rb_eArgError,"argument or method missing");
    return Qnil;
}


void
Init_math()
{
    VALUE hCast;

    mNMath = rb_define_module_under(mNum, "NMath");
    rb_define_singleton_method(mNMath, "method_missing", num_math_method_missing, -1);

    mNMathDFloat = rb_define_module_under(mNMath, "DFloat");
    mNMathDComplex = rb_define_module_under(mNMath, "DComplex");

    hCast = rb_hash_new();
    rb_define_const(mNMath, "TYPE_MODULE", hCast);
    rb_hash_aset(hCast, cInt64,      mNMathDFloat);
    rb_hash_aset(hCast, cInt48,      mNMathDFloat);
    rb_hash_aset(hCast, cInt32,      mNMathDFloat);
    rb_hash_aset(hCast, cInt24,      mNMathDFloat);
    rb_hash_aset(hCast, cInt16,      mNMathDFloat);
    rb_hash_aset(hCast, cInt8,       mNMathDFloat);
    rb_hash_aset(hCast, cUInt64,     mNMathDFloat);
    rb_hash_aset(hCast, cUInt48,     mNMathDFloat);
    rb_hash_aset(hCast, cUInt32,     mNMathDFloat);
    rb_hash_aset(hCast, cUInt24,     mNMathDFloat);
    rb_hash_aset(hCast, cUInt16,     mNMathDFloat);
    rb_hash_aset(hCast, cUInt8,      mNMathDFloat);
    rb_hash_aset(hCast, cDFloat,     mNMathDFloat);
    rb_hash_aset(hCast, cDFloat,     mNMathDFloat);
    rb_hash_aset(hCast, cDComplex,   mNMathDComplex);
    rb_hash_aset(hCast, rb_cFixnum,  rb_mMath);
    rb_hash_aset(hCast, rb_cBignum,  rb_mMath);
    rb_hash_aset(hCast, rb_cInteger, rb_mMath);
    rb_hash_aset(hCast, rb_cFloat,   rb_mMath);
    rb_hash_aset(hCast, cComplex,    mNMathDComplex);

    id_send = rb_intern("send");
}
