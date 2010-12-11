/*
  na_array.c
  Numerical Array Extention for Ruby
    (C) Copyright 1999-2003 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#include <ruby.h>
#include "narray.h"
//#include "narray_local.h"

/* Multi-Dimensional Array Investigation */
typedef struct {
  size_t shape;
  VALUE  val;
} na_mdai_item_t;

typedef struct {
    int   n;
    na_mdai_item_t *item;
    int   type;
    VALUE datatype;
    VALUE int_max;
} na_mdai_t;

enum { NA_NONE, NA_BIT, NA_INT32, NA_INT64, NA_DFLOAT, NA_DCOMPLEX, NA_ROBJ, NA_NTYPES };
//Bit, Int32, Int64, DFloat, DComplex, RObject


void na_object_type_(na_mdai_t *mdai, VALUE v)
{
    static VALUE int32_max = Qnil;
    if (NIL_P(int32_max))
	int32_max = ULONG2NUM(2147483647);

    switch(TYPE(v)) {

    case T_TRUE:
    case T_FALSE:
	if (mdai->type<NA_BIT)
	    mdai->type = NA_BIT;
	return;

#if SIZEOF_LONG == 4
    case T_FIXNUM:
	if (mdai->type<NA_INT32)
	    mdai->type = NA_INT32;
	return;
    case T_BIGNUM:
	if (mdai->type<NA_INT64) {
	    v = rb_funcall(v,rb_intern("abs"),0);
	    if (RTEST(rb_funcall(v,rb_intern("<="),1,int32_max))) {
		if (mdai->type<NA_INT32)
		    mdai->type = NA_INT32;
	    } else {
		mdai->type = NA_INT64;
	    }
	}
	return;

#elif SIZEOF_LONG == 8
    case T_FIXNUM:
	if (mdai->type<NA_INT64) {
	    long x = NUM2LONG(v);
	    if (x<0) x=-x;
	    if (x<=2147483647) {
		if (mdai->type<NA_INT32)
		    mdai->type = NA_INT32;
	    } else {
		mdai->type = NA_INT64;
	    }
	}
	return;
    case T_BIGNUM:
	if (mdai->type<NA_INT64)
	    mdai->type = NA_INT64;
	return;
#else
    case T_FIXNUM:
    case T_BIGNUM:
	if (mdai->type<NA_INT64) {
	    v = rb_funcall(v,rb_intern("abs"),0);
	    if (RTEST(rb_funcall(v,rb_intern("<="),1,int32_max))) {
		if (mdai->type<NA_INT32)
		    mdai->type = NA_INT32;
	    } else {
		mdai->type = NA_INT64;
	    }
	}
	return;
#endif

    case T_FLOAT:
	if (mdai->type<NA_DFLOAT)
	    mdai->type = NA_DFLOAT;
	return;

    case T_NIL:
	return;

    default:
	if (IsNArray(v)) {
	    if (NIL_P(mdai->datatype)) {
		mdai->datatype = CLASS_OF(v);
	    } else {
	    	mdai->datatype = rb_funcall(CLASS_OF(v),
					    rb_intern("cast_type"),
					    1,mdai->datatype);
	    }
	    return;
	}
	if (CLASS_OF(v) == rb_const_get( rb_cObject, rb_intern("Complex") )) {
	    mdai->type = NA_DCOMPLEX;
	    return;
	}
    }
    mdai->type = NA_ROBJ;
}


VALUE
 na_object_type(int type, VALUE v)
{
    static VALUE int32_max = Qnil;
    if (NIL_P(int32_max))
	int32_max = ULONG2NUM(2147483647);

    switch(TYPE(v)) {

    case T_TRUE:
    case T_FALSE:
	if (type<NA_BIT)
	    return NA_BIT;
	return type;

#if SIZEOF_LONG == 4
    case T_FIXNUM:
	if (type<NA_INT32)
	    return NA_INT32;
	return type;
    case T_BIGNUM:
	if (type<NA_INT64) {
	    v = rb_funcall(v,rb_intern("abs"),0);
	    if (RTEST(rb_funcall(v,rb_intern("<="),1,int32_max))) {
		if (type<NA_INT32)
		    return NA_INT32;
	    } else {
		return NA_INT64;
	    }
	}
	return type;

#elif SIZEOF_LONG == 8
    case T_FIXNUM:
	if (type<NA_INT64) {
	    long x = NUM2LONG(v);
	    if (x<0) x=-x;
	    if (x<=2147483647) {
		if (type<NA_INT32)
		    return NA_INT32;
	    } else {
		return NA_INT64;
	    }
	}
	return type;
    case T_BIGNUM:
	if (type<NA_INT64)
	    return NA_INT64;
	return type;
#else
    case T_FIXNUM:
    case T_BIGNUM:
	if (type<NA_INT64) {
	    v = rb_funcall(v,rb_intern("abs"),0);
	    if (RTEST(rb_funcall(v,rb_intern("<="),1,int32_max))) {
		if (type<NA_INT32)
		    return NA_INT32;
	    } else {
		return NA_INT64;
	    }
	}
	return type;
#endif

    case T_FLOAT:
	if (type<NA_DFLOAT)
	    return NA_DFLOAT;
	return type;

    case T_NIL:
	return type;

    default:
	if (CLASS_OF(v) == rb_const_get( rb_cObject, rb_intern("Complex") )) {
	    return NA_DCOMPLEX;
	}
    }
    return NA_ROBJ;
}


void na_mdai_object_type(na_mdai_t *mdai, VALUE v)
{
    if (IsNArray(v)) {
	if (NIL_P(mdai->datatype)) {
	    mdai->datatype = CLASS_OF(v);
	} else {
	    mdai->datatype = rb_funcall(CLASS_OF(v),rb_intern("cast_type"),
					1,mdai->datatype);
	}
    } else {
	mdai->type = na_object_type(mdai->type,v);
    }
}


static na_mdai_t *
  na_mdai_alloc(VALUE ary)
{
    int i, n=2;
    na_mdai_t *mdai;

    mdai = ALLOC(na_mdai_t);
    mdai->n = n;
    mdai->item = ALLOC_N( na_mdai_item_t, n );
    for (i=0; i<n; i++) {
	mdai->item[i].shape = 0;
	mdai->item[i].val = Qnil;
    }
    mdai->item[0].val = ary;
    //mdai->type = ALLOC_N( int, NA_NTYPES );
    //for (i=0; i<NA_NTYPES; i++)
    //	mdai->type[i]=0;
    mdai->type = NA_NONE;
    mdai->datatype = Qnil;

    return mdai;
}

static void
na_mdai_realloc(na_mdai_t *mdai, int n_extra)
{
    int i, n;

    i = mdai->n;
    mdai->n += n_extra;
    n = mdai->n;
    REALLOC_N( mdai->item, na_mdai_item_t, n );
    for (; i<n; i++) {
	mdai->item[i].shape = 0;
	mdai->item[i].val = Qnil;
    }
}

static size_t *
na_mdai_free(na_mdai_t *mdai, int *ndim, VALUE *type)
{
    int i, t, r;
    size_t *shape=NULL;
    VALUE tp;

    //for (t=i=NA_BYTE; i<NA_NTYPES; i++) {
    //  if ( mdai->type[i] > 0 )
    //    t = na_upcast[t][i];
    //}
    //*type = t;

    // Dimension
    for (i=0; i < mdai->n && mdai->item[i].shape > 0; i++) ;
    //for (i=0; i < mdai->n; i++) ;
    *ndim = i;

    if (*ndim>0) {
	// Shape
	shape = ALLOC_N(size_t,i);
	//for (i=0; r-->0; i++) {
	for (i=0; i<*ndim; i++) {
	    //shape[i] = mdai->item[r].shape;
	    shape[i] = mdai->item[i].shape;
	}
	// DataType
	switch(mdai->type) {
	case NA_BIT:
	    tp = cBit;
	    break;
	case NA_INT32:
	    tp = cInt32;
	    break;
	case NA_INT64:
	    tp = cInt64;
	    break;
	case NA_DFLOAT:
	    tp = cDFloat;
	    break;
	case NA_DCOMPLEX:
	    tp = cDComplex;
	    break;
	case NA_ROBJ:
	    tp = cRObject;
	    break;
	default:
	    tp = Qnil;
	}
	if (!NIL_P(mdai->datatype)) {
	    if (NIL_P(tp)) {
		tp = mdai->datatype;
	    } else {
		tp = rb_funcall(mdai->datatype,rb_intern("cast_type"),1,tp);
	    }
	}
	*type = tp;
    }
    //xfree(mdai->type);
    xfree(mdai->item);
    xfree(mdai);
    return shape;
}


#define EXCL(r) (RTEST(rb_funcall((r),rb_intern("exclude_end?"),0)))

/* Range as a Sequence of numbers */
static void
na_range_to_sequence(VALUE obj, size_t *n, size_t *beg, size_t *step)
{
    int end,len;

    *beg = NUM2INT(rb_ivar_get(obj, rb_intern("begin")));
    end = NUM2INT(rb_ivar_get(obj, rb_intern("end")));
    len = end - *beg;

    /* direction */
    if (len>0) {
	*step = 1;
	if (EXCL(obj)) end--; else len++;
    }
    else if (len<0) {
	len   = -len;
	*step = -1;
	if (EXCL(obj)) end++; else len++;
    }
    else /*if(len==0)*/ {
	*step = 0;
	if (!EXCL(obj)) {
	    len++;
	}
    } 
    *n = len;
}


/* investigate ndim, shape, type of Array */
static int
na_mdai_investigate(na_mdai_t *mdai, int ndim)
{
    size_t i, j, len, length;
    double dbeg, dstep;
    VALUE  v;
    VALUE  val;
    //struct RArray *ary;

    val = mdai->item[ndim-1].val;
    //ary = RARRAY(mdai->item[ndim-1].val);
    //len = ary->len;
    len = RARRAY_LEN(val);

    //for (i=0; i < ary->len; i++) {
    for (i=0; i < RARRAY_LEN(val); i++) {

        //v = ary->ptr[i];
        v = RARRAY_PTR(val)[i];

	if (TYPE(v) == T_ARRAY) {
	    /* check recursive array */
	    for (j=0; j<ndim; j++) {
		if (mdai->item[j].val == v)
		    rb_raise(rb_eStandardError,
			     "cannot convert from a recursive Array to NArray");
	    }
	    if ( ndim >= mdai->n ) {
		na_mdai_realloc(mdai,2);
	    }
	    mdai->item[ndim].val = v;
	    if ( na_mdai_investigate(mdai,ndim+1) ) {
		len--; /* Array is empty */
	    }
	}
	else
	//if ( rb_obj_is_kind_of(v, rb_cRange) ) {
	//    na_range_to_sequence(v,&length,&start,&dir);
        if (rb_obj_is_kind_of(v, rb_cRange) || rb_obj_is_kind_of(v, num_cStep)) {
	    num_step_sequence(v,&length,&dbeg,&dstep);
	    len += length-1;
	    //mdai->type[ na_object_type(rb_ivar_get(v, rb_intern("beg"))) ] = 1;
	    //mdai->type[ na_object_type(rb_ivar_get(v, rb_intern("end"))) ] = 1;
	    na_mdai_object_type(mdai,rb_ivar_get(v, rb_intern("begin")));
	    na_mdai_object_type(mdai,rb_ivar_get(v, rb_intern("end")));
	}
	else {
		
	    //mdai->type[ na_object_type(v) ] = 1;
	    na_mdai_object_type(mdai,v);

	    if (IsNArray(v)) {
		int r;
		narray_t *na;
		GetNArray(v,na);
		if ( na->ndim == 0 ) {
		    len--; /* NArray is empty */
		} else {
		    if ( ndim+na->ndim > mdai->n ) {
			na_mdai_realloc(mdai,((na->ndim-1)/4+1)*4);
		    }
		    //for ( j=na->ndim, r=ndim; j-- > 0  ; r++ ) {
		    for ( j=0,r=ndim; j < na->ndim  ; j++,r++ ) {
			if ( mdai->item[r].shape < na->shape[j] )
			    mdai->item[r].shape = na->shape[j];
		    }
		}
	    }
	}
    }

    if (len==0) return 1; /* this array is empty */
    if (mdai->item[ndim-1].shape < len) {
	mdai->item[ndim-1].shape = len;
    }
    return 0;
}


// /* get index from multiple-index  */
// static int
//  na_index_pos(struct NARRAY *ary, int *idxs)
// {
//   int i, idx, pos = 0;
// 
//   for ( i = ary->ndim; (i--)>0; ) {
//     idx = idxs[i];
//     if (idx < 0 || ary->shape[i] <= idx) {
//       abort();
//       rb_raise(rb_eRuntimeError,
// 	       "Subsctipt out of range: accessing shape[%i]=%i with %i",
// 	       i, ary->shape[i], idx );
//     }
//     pos = pos * ary->shape[i] + idx;
//   }
//   return pos;
// }
// 
// 
// static void
//  na_copy_nary_to_nary(VALUE obj, struct NARRAY *dst,
// 		      int thisndim, int *idx)
// {
//   struct NARRAY *src;
//   struct slice *s;
//   int  i, n;
// 
//   GetNArray(obj,src);
//   n = thisndim - src->ndim + 1;
// 
//   s = ALLOCA_N(struct slice, dst->ndim+1);
//   for (i=0; i < n; i++) {
//     s[i].n    = 1;
//     s[i].beg  = 0;
//     s[i].step = 0;
//     s[i].idx  = NULL;
//   }
//   for (   ; i <= thisndim; i++) {
//     s[i].n    = src->shape[i-n];
//     s[i].beg  = 0;
//     s[i].step = 1;
//     s[i].idx  = NULL;
//   }
//   for (   ; i < dst->ndim; i++) {
//     s[i].n    = 1;
//     s[i].beg  = idx[i];
//     s[i].step = 0;
//     s[i].idx  = NULL;
//   }
//   na_aset_slice(dst,src,s);
// }
// 
// 
// /* copy Array to NArray */
// static void
//  na_copy_ary_to_nary( struct RArray *ary, struct NARRAY *na,
// 		      int thisndim, int *idx, int type )
// {
//   int i, j, pos, len, start, step, dir;
//   VALUE v;
// 
//   if (thisndim==0) {
//     for (i = idx[0] = 0; i < ary->len; i++) {
//       v = ary->ptr[i];
//       if (rb_obj_is_kind_of(v, rb_cRange)) {
// 	na_range_to_sequence(v,&len,&start,&dir);
// 	if (len>0) {
// 	  pos = na_index_pos(na,idx);
// 	  IndGenFuncs[type](len, NA_PTR(na,pos),na_sizeof[type], start,dir);
// 	  idx[0] += len;
// 	}
//       }
//       else if (TYPE(v) != T_ARRAY) {
// 	/* NIL if empty */
// 	if (v != Qnil) {
// 	  pos = na_index_pos(na,idx);
// 	  SetFuncs[type][NA_ROBJ]( 1, NA_PTR(na,pos), 0, &v, 0 );
// 	  /* copy here */
// 	}
// 	idx[0] ++;
//       }
//     }
//   }
//   else /* thisndim > 0 */
//   { 
//     for (i = idx[thisndim] = 0; i < ary->len; i++) {
//       v = ary->ptr[i];
//       if (TYPE(v) == T_ARRAY) {
// 	na_copy_ary_to_nary(RARRAY(v),na,thisndim-1,idx,type);
// 	if (idx[thisndim-1]>0) idx[thisndim]++;
//       }
//       else if (IsNArray(v)) {
// 	na_copy_nary_to_nary(v,na,thisndim-1,idx);
// 	idx[thisndim]++;
//       }
//       else {
// 	for (j=thisndim; j; ) idx[--j] = 0;
// 
// 	if (rb_obj_is_kind_of(v, rb_cRange)) {
// 	  na_range_to_sequence(v,&len,&start,&dir);
// 	  if (len>0) {
// 	    pos = na_index_pos(na,idx);
// 	    idx[thisndim]++;
// 	    step = na_index_pos(na,idx)-pos;
// 	    IndGenFuncs[type]( len, NA_PTR(na,pos), na_sizeof[type]*step,
// 			       start, dir );
// 	    idx[thisndim] += len-1;
// 	  }
// 	}
// 	else {
// 	  pos = na_index_pos(na,idx);
// 	  SetFuncs[type][NA_ROBJ]( 1, NA_PTR(na,pos), 0, ary->ptr+i, 0 );
// 	  idx[thisndim]++;
// 	}
// 	/* copy here */
//       }
//     }
//   }
// }
// 


static VALUE
na_s_array_shape(VALUE mod, VALUE ary)
{
    int  i, ndim;
    VALUE type=Qnil;
    size_t *shape;
    na_mdai_t *mdai;
    VALUE vshape;

    if (TYPE(ary)!=T_ARRAY) {
	// 0-dimension
	return rb_ary_new();
    }
    //if (RARRAY(ary)->len < 1) {
    //	// empty array
    //	vshape = rb_ary_new();
    //	rb_ary_push( vshape, INT2FIX(0) );
    //	return vshape;
    //}
    // investigate MD-Array
    mdai = na_mdai_alloc(ary);
    na_mdai_investigate(mdai,1);
    // obtain properties
    shape = na_mdai_free(mdai,&ndim,&type);
    // make shape object
    vshape = rb_ary_new();
    for (i=0; i<ndim; i++) {
	rb_ary_push( vshape, SIZE2NUM(shape[i]) );
    }
    xfree(shape);
    return vshape;
}


VALUE
na_array_type(VALUE ary)
{
    int   ndim;
    VALUE type=Qnil;
    size_t *shape;
    na_mdai_t *mdai;

    switch(TYPE(ary)) {
    case T_ARRAY:
	mdai = na_mdai_alloc(ary);
	na_mdai_investigate(mdai,1);
	shape = na_mdai_free(mdai,&ndim,&type);
	xfree(shape);
	return type;
    }
    return CLASS_OF(ary);
    //// make shape object
    //vshape = rb_ary_new();
    //for (i=0; i<ndim; i++) {
    //	rb_ary_push( vshape, SIZE2NUM(shape[i]) );
    //}
    //xfree(shape);
    //return vshape;
}

static VALUE
na_s_array_type(VALUE mod, VALUE ary)
{
    return na_array_type(ary);
}


static VALUE
na_mdai(VALUE mod, VALUE ary)
{
    int  i, ndim;
    VALUE type=Qnil;
    size_t *shape, *idx;
    na_mdai_t *mdai;
    narray_t *na;
    VALUE v;

    /* empty array */
    //if (RARRAY(ary)->len < 1) {
    //	return Qnil; //na_make_empty( type, klass );
    //}
    if (TYPE(ary)!=T_ARRAY) {
	printf("ndim=%d\n",0);
	return Qnil;
    }
    mdai  = na_mdai_alloc(ary);
    na_mdai_investigate(mdai,1);
    shape = na_mdai_free(mdai,&ndim,&type);
    printf("\ntype=%lx\n",type);
    //printf("datatype=%d\n",datatype);
    printf("ndim=%d\n",ndim);
    for (i=0; i<ndim; i++) {
	printf(" shape[%d]=%ld\n",i,shape[i]);
    }
    return Qnil;
    //return rb_narray_new( type, ndim, shape );
}


size_t *
na_mdarray_investigate(VALUE ary, int *ndim, VALUE *type)
{
    int f;
    size_t *shape;
    na_mdai_t *mdai;

    if (TYPE(ary) != T_ARRAY) {
	puts("not Array");
	*ndim = 0;
	return NULL;
    }
    mdai = na_mdai_alloc(ary);
    f = na_mdai_investigate(mdai,1);
    shape = na_mdai_free(mdai,ndim,type);
    if (f) {
	*ndim = 1;
	shape = ALLOC(size_t);
	shape[0] = 0;
    }
    return shape;
}

Init_na_array()
{
    rb_define_singleton_method(cNArray, "mdai", na_mdai, 1);
    rb_define_singleton_method(cNArray, "array_shape", na_s_array_shape, 1);
    rb_define_singleton_method(cNArray, "array_type", na_s_array_type, 1);
}


// static VALUE
//  na_ary_to_nary_w_type(VALUE ary, int type_spec, VALUE klass)
// {
//   int  i, ndim;
//   int  type = NA_BYTE;
//   int *shape, *idx;
//   na_mdai_t *mdai;
//   struct NARRAY *na;
//   VALUE v;
// 
//   /* empty array */
//   if (RARRAY(ary)->len < 1) {
//     return na_make_empty( type, klass );
//   }
// 
//   mdai  = na_mdai_alloc(ary);
//   na_mdai_investigate(mdai,1);
//   shape = na_mdai_free(mdai,&ndim,&type);
// 
//   /*
//   printf("ndim=%i\n", ndim);
//   printf("type=%i\n", type);
//   for (i=0; i<ndim; i++) {
//     printf("shape[%i]=%i\n", i, shape[i]);
//   }
//   */
// 
//   /* type specification */
//   if (type_spec!=NA_NONE)
//     type = type_spec;
// 
//   /* empty array */
//   if (ndim==0)
//     return na_make_empty( type, klass );
// 
//   /* Create NArray */
//   v  = na_make_object(type,ndim,shape,klass);
//   xfree(shape);
// 
//   GetNArray(v,na);
//   na_clear_data(na);
// 
//   idx = ALLOCA_N(int,ndim);
//   for (i=0; i<ndim; i++) idx[i]=0;
// 
//   na_copy_ary_to_nary( RARRAY(ary), na, ndim-1, idx, type );
// 
//   return v;
// }
// 
// 
// VALUE
//  na_ary_to_nary(VALUE ary, VALUE klass)
// {
//   return na_ary_to_nary_w_type( ary, NA_NONE, klass );
// }


// /* obj.kind_of?(NArray) == true */
// 
// VALUE
//  na_dup_w_type(VALUE v2, int type)
// {
//   VALUE  v1;
//   struct NARRAY *a1, *a2;
// 
//   GetNArray(v2,a2);
//   v1 = na_make_object(type, a2->ndim, a2->shape, CLASS_OF(v2));
//   GetNArray(v1,a1);
//   na_copy_nary(a1,a2);
//   return v1;
// }
// 
// 
// VALUE
//  na_change_type(VALUE obj, int type)
// {
//   struct NARRAY *a2;
// 
//   GetNArray(obj,a2);
// 
//   if (a2->type == type)
//     return obj;
// 
//   return na_dup_w_type(obj, type);
// }
// 
// 
// VALUE
//  na_upcast_type(VALUE obj, int type)  /* na_upcast_narray */
// {
//   int newtype;
//   struct NARRAY *a2;
// 
//   GetNArray(obj,a2);
//   newtype = na_upcast[a2->type][type];
// 
//   if (newtype == a2->type)
//     return obj;
// 
//   return na_dup_w_type(obj, newtype);
// }
// 
// 
// /* obj.kind_of?(Object) == true */
// 
// VALUE
//  na_cast_object(VALUE obj, int type) /* na_cast_certain */
// {
//   if (IsNArray(obj)) {
//     return na_change_type(obj,type);
//   }
//   if (TYPE(obj) == T_ARRAY) {
//     return na_ary_to_nary_w_type(obj,type,cNArray);
//   }
//   return na_make_scalar(obj,type);
// }
// 
// 
// VALUE
//  na_cast_unless_narray(VALUE obj, int type)
// {
//   if (IsNArray(obj)) {
//     return obj;
//   }
//   if (TYPE(obj) == T_ARRAY) {
//     return na_ary_to_nary_w_type(obj,type,cNArray);
//   }
//   return na_make_scalar(obj,type);
// }
// 
// 
// VALUE
//  na_cast_unless_array(VALUE obj, int type)
// {
//   if (IsNArray(obj)) {
//     return obj;
//   }
//   if (TYPE(obj) == T_ARRAY) {
//     return na_ary_to_nary(obj,cNArray);
//   }
//   return na_make_scalar(obj,type);
// }
// 
// 
// VALUE
//  na_upcast_object(VALUE obj, int type)
// {
//   if (IsNArray(obj)) {
//     return na_upcast_type(obj,type);
//   }
//   if (TYPE(obj) == T_ARRAY) {
//     return na_ary_to_nary_w_type(obj,type,cNArray);
//   }
//   return na_make_scalar(obj,type);
// }
// 
// 
// VALUE
//  na_to_narray(VALUE obj)
// {
//   if (IsNArray(obj)) {
//     return obj;
//   }
//   if (TYPE(obj) == T_ARRAY) {
//     return na_ary_to_nary(obj,cNArray);
//   }
//   return na_make_scalar(obj,na_object_type(obj));
// }
// 
// 
// /* convert NArray to Array */
// static VALUE
//  na_to_array0(struct NARRAY* na, int *idx, int thisndim, void (*func)())
// {
//   int i, elmsz;
//   char *ptr;
//   VALUE ary, val;
// 
//   /* Create New Array */
//   ary = rb_ary_new2(na->shape[thisndim]);
// 
//   if (thisndim == 0) {
//     ptr   = NA_PTR( na, na_index_pos(na,idx) );
//     elmsz = na_sizeof[na->type];
//     for (i = na->shape[0]; i; i--) {
//       (*func)( 1, &val, 0, ptr, 0 );
//       ptr += elmsz;
//       rb_ary_push( ary, val );
//     }
//   }
//   else {
//     for (i = 0; i < na->shape[thisndim]; i++) {
//       idx[thisndim] = i;
//       rb_ary_push( ary, na_to_array0(na,idx,thisndim-1,func) );
//     }
//   }
//   return ary;
// }
// 
// 
// /* method: to_a -- convert itself to Array */
// VALUE
//  na_to_array(VALUE obj)
// {
//   struct NARRAY *na;
//   int *idx, i;
// 
//   GetNArray(obj,na);
// 
//   if (na->ndim<1)
//     return rb_ary_new();
// 
//   idx = ALLOCA_N(int,na->ndim);
//   for (i = 0; i<na->ndim; i++) idx[i] = 0;
//   return na_to_array0(na,idx,na->ndim-1,SetFuncs[NA_ROBJ][na->type]);
// }
// 
// 
// static VALUE
//  na_inspect_col( int n, char *p2, int p2step, void (*tostr)(),
// 		 VALUE sep, int ndim )
// {
//   VALUE str=Qnil, tmp;
//   int max_col = 77;
//   int sep_len = RSTRING(sep)->len;
// 
//   if (n>0)
//     (*tostr)(&str,p2);
// 
//   for (n--; n>0; n--) {
//     p2 += p2step;
//     (*tostr)(&tmp,p2);
// 
//     if (!NIL_P(sep)) rb_str_concat(str, sep);
// 
//     if (RSTRING(str)->len + RSTRING(tmp)->len + ndim*4 + sep_len < max_col) {
//       rb_str_concat(str, tmp);
//     } else {
//       rb_str_cat(str,"...",3);
//       return str;
//     }
//   }
//   return str;
// }
// 
// 
// /*
//  *   Create inspect string ... under construction
//  */
// 
// VALUE
//  na_make_inspect(VALUE val)
// {
//   int   i, ii, ndim, count_line=0, max_line=10;
//   int  *si;
//   struct NARRAY *ary;
//   struct slice *s1;
// 
//   VALUE fs = rb_str_new(", ",2);
// 
//   GetNArray(val,ary);
//   if (ary->total < 1) return rb_str_new(0, 0);
// 
//   /* Allocate Structure */
//   ndim = ary->ndim;
//   s1 = ALLOCA_N(struct slice, ndim+1);
//   si = ALLOCA_N(int,ndim);
//   na_set_slice_1obj(ndim,s1,ary->shape);
// 
//   /* Iteration */
//   na_init_slice(s1, ndim, ary->shape, na_sizeof[ary->type]);
//   i = ndim;
//   s1[i].p = ary->ptr;
//   val = rb_str_new(0,0);
//   for(;;) {
//     /* set pointers */
//     while (i > 0) {
//       i--;
//       rb_str_cat(val, "[ ", 2);
//       s1[i].p = s1[i].pbeg + s1[i+1].p;
//       si[i] = s1[i].n;
//     }
// 
//     rb_str_concat(val, na_inspect_col( s1[0].n, s1[0].p, s1[0].pstep,
// 				       InspFuncs[ary->type], fs, ndim ));
// 
//     /* ndim up */
//     do {
//       rb_str_cat(val, " ]", 2);
//       if ( ++i == ndim ) return val;
//     } while ( --si[i] == 0 );
//     s1[i].p += s1[i].pstep;
// 
//     rb_str_concat(val, fs);
//     rb_str_cat(val, "\n", 1);
// 
//     /* count check */
//     if (++count_line>=max_line) {
//       rb_str_cat(val, " ...", 4);
//       return val;
//     }
//     /* indent */
//     for (ii=i; ii<ndim; ii++)
//       rb_str_cat(val, "  ", 2);
//   }
// }
