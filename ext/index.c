/*
  index.c
  Numerical Array Extention for Ruby
    (C) Copyright 1999-2007 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#define NARRAY_C
#include <ruby.h>
//#include <version.h>
#include "narray.h"
//#include "narray_local.h"
//#include "bytedata.h"

#include "template.h"


#define MATCH_INT(s) (RTEST(rb_funcall(rb_eval_string("/^[-+]?\\d+$/"),rb_intern("match"),1,s)))
#define MATCH_ALL(s) (RTEST(rb_funcall(rb_eval_string("/^(\\*|all)?$/"),rb_intern("match"),1,s)))
#define MATCH_REV(s) (RTEST(rb_funcall(rb_eval_string("/^rev(erse)?$/"),rb_intern("match"),1,s)))
#define MATCH_NEW(s) (RTEST(rb_funcall(rb_eval_string("/^-|new$/"),rb_intern("match"),1,s)))
#define MATCH_REST(s) (RTEST(rb_funcall(rb_eval_string("/^~|rest$/"),rb_intern("match"),1,s)))
#define MATCH_MARK(s) (RTEST(rb_funcall(rb_eval_string("/^\\+|mark|sum$/"),rb_intern("match"),1,s)))


typedef struct {
    size_t  n;
    size_t  beg;
    ssize_t step;
    size_t *idx;
    int     mark;
    int     orig_dim;
} na_index_arg_t;


static ID id_ast;
static ID id_all;
static ID id_minus;
static ID id_new;
static ID id_reverse;
static ID id_plus;
static ID id_mark;
static ID id_sum;
static ID id_tilde;
static ID id_rest;
static ID id_beg;
static ID id_end;
static ID id_exclude_end;




static int
na_parse_index_preprocess(VALUE args, int na_ndim)
{
    int i, j, count_new=0, count_rest=0;
    ID id;
    volatile VALUE a;

    for (i=0; i<RARRAY_LEN(args); ) {
	a = RARRAY_PTR(args)[i];

	switch (TYPE(a)) {

	case T_STRING:
	    a = rb_funcall(a,rb_intern("split"),2,rb_str_new2(","),INT2FIX(-1));
	    for (j=0; j<RARRAY_LEN(a); j++) {
		if (MATCH_NEW(RARRAY_PTR(a)[j])) {
		    RARRAY_PTR(a)[j] = ID2SYM(id_new);
		    count_new++;
		} else
		if (MATCH_REST(RARRAY_PTR(a)[j])) {
		    RARRAY_PTR(a)[j] = Qfalse;
		    count_rest++;
		}
	    }
	    rb_funcall(args, rb_intern("[]="), 3, INT2FIX(i), INT2FIX(1), a);
	    i += RARRAY_LEN(a);
	    break;

	case T_SYMBOL:
	    id = SYM2ID(a);
	    if (id==id_new || id==id_minus) {
		RARRAY_PTR(args)[i] = ID2SYM(id_new);
		count_new++;
	    }
	    if (id==id_rest || id==id_tilde) {
		RARRAY_PTR(args)[i] = Qfalse;
		count_rest++;
	    }
	default:
	    i++;
	}
    }

    if (count_rest>1)
	rb_raise(rb_eIndexError, "multiple rest-dimension is not allowd");

    if (count_rest==0 && count_new==0 && i==1)
	return 0;

    if (count_rest==0 && i-count_new != na_ndim)
	rb_raise(rb_eIndexError, "# of index=%i != narray.ndim=%i",
		 i-count_new, na_ndim);

    return count_new;
}


void
na_set_index_step(na_index_arg_t *q, int i, size_t n, size_t beg, ssize_t step)
{
    q->n    = n;
    q->beg  = beg;
    q->step = step;
    q->idx  = NULL;
    q->mark = 0;
    q->orig_dim = i;
}


void
na_set_index_scalar(na_index_arg_t *q, int i, ssize_t size, ssize_t x)
{
    if ( x < -size || x >= size )
	rb_raise( rb_eRangeError,
		  "array index (%"SZF"d) is out of array size (%"SZF"d)",
		  x, size );
    if ( x < 0 )
	x += size;
    q->n    = 1;
    q->beg  = x;
    q->step = 0;
    q->idx  = NULL;
    q->mark = 0;
    q->orig_dim = i;
}


static int
na_parse_each_index(volatile VALUE a, ssize_t size, int i, na_index_arg_t *q)
{
    int k;
    ssize_t beg, end, step, n, x, *idx;
    int rest_dim=0;
    ID id;
    volatile VALUE s;

    switch(TYPE(a)) {

    case T_FIXNUM:
	na_set_index_scalar(q,i,size,FIX2LONG(a));
	break;

    case T_BIGNUM:
        na_set_index_scalar(q,i,size,NUM2SSIZE(a));
        break;

    case T_FLOAT:
        na_set_index_scalar(q,i,size,NUM2SSIZE(a));
        break;

    case T_NIL:
    case T_TRUE:
        na_set_index_step(q,i,size,0,1);
        break;

    case T_SYMBOL:
        id = SYM2ID(a);
        if (id==id_all || id==id_ast) {
	    na_set_index_step(q,i,size,0,1);
        }
	else if (id==id_reverse) {
	    na_set_index_step(q,i,size,size-1,-1);
        }
	else if (id==id_new) {
	    na_set_index_step(q,i,1,0,1);
        }
	else if (id==id_mark || id==id_sum || id==id_plus) {
	    na_set_index_step(q,i,size,0,1);
	    q->mark = 1;
        }	    
        break;

        /*
    case T_ARRAY:
        idx = na_cast_object(idx,NA_LINT);
        GetNArray(idx,na);
        size = na_ary_to_index(na,shape,sl);

        q[j].n    = array_size;
        q[j].beg  = 0;
        q[j].step = 1;
        q[j].idx  = index;
        q[j].orig_dim = i;
        total *= array_size;
        break;

        return size;
        */

    case T_ARRAY:
	n = RARRAY_LEN(a);
	idx = ALLOC_N(size_t, n);
	printf("array size n =%ld\n",n);
	for (k=0; k<n; k++) {
	    x = NUM2SIZE(RARRAY_PTR(a)[k]);
	    // range check
	    if ( x < -size || x >= size )
		rb_raise( rb_eRangeError,
			  " array index[%ld]=%ld is out of array size (%ld)",
			  k, x, size );
	    if ( x < 0 )
		x += size;
	    idx[k] = x;
	}
	q->n    = n;
        q->beg  = 0;
        q->step = 1;
        q->idx  = idx;
	q->mark = 0;
        q->orig_dim = i;
	break;

    case T_STRING:
        a = rb_funcall(a,rb_intern("strip"),0);
        a = rb_funcall(a,rb_intern("split"),2,rb_str_new2(":"),INT2FIX(-1));
        step = 1;
        switch (RARRAY_LEN(a)){
        case 3:
	    s = RARRAY_PTR(a)[2];
	    s = rb_funcall(s,rb_intern("strip"),0);
	    if (MATCH_ALL(s)) {
		step = 1;
	    }
	    else if (MATCH_INT(s)) {
		step = NUM2SSIZE(rb_str_to_inum(s,10,0));
		if (step==0) {
		    rb_raise(rb_eIndexError,"step must be non-zero");
		}
	    }
	    else {
		rb_raise(rb_eIndexError,"invalid step in colon range '%s'",
			 StringValuePtr(s));
	    }

        case 2:
	    s = RARRAY_PTR(a)[1];
	    s = rb_funcall(s,rb_intern("strip"),0);
	    if (MATCH_ALL(s)) {
		if (step>0)
		    end = -1;
		else
		    end = 0;
	    }
	    else if (MATCH_INT(s)) {
		end = NUM2SSIZE(rb_str_to_inum(s,10,0));
	    }
	    else {
		rb_raise(rb_eIndexError,"invalid end in colon range '%s'",
			 StringValuePtr(s));
	    }
	    if (end<0)
		end += size;

	    s = RARRAY_PTR(a)[0];
	    s = rb_funcall(s,rb_intern("strip"),0);
	    if (MATCH_ALL(s)) {
		if (step>0)
		    beg = 0;
		else
		    beg = -1;
	    }
	    else if (MATCH_INT(s)) {
		beg = NUM2SSIZE(rb_str_to_inum(s,10,0));
	    }
	    else {
		rb_raise(rb_eIndexError,"invalid start in colon range '%s'",
			 StringValuePtr(s));
	    }
	    if (beg<0)
		beg += size;

	    if ( beg < -size || beg >= size || 
		 end < -size || end >= size )
		rb_raise( rb_eRangeError,
			  "beg=%ld,end=%ld is out of array size (%ld)",
			  beg, end, size );

	    //n = (end-beg+1)/step;
	    n = (end-beg)/step+1;
	    if (n<0) n=0;

	    na_set_index_step(q,i,n,beg,step);

	    break;

        case 1:
	    s = RARRAY_PTR(a)[0];
	    //s = na_parse_index_string_item(s);

	    if (MATCH_ALL(s)) {
		na_set_index_step(q,i,size,0,1);
	    } else
	    if (MATCH_MARK(s)) {
		na_set_index_step(q,i,size,0,1);
		q->mark = 1;
	    } else
	    if (MATCH_REV(s)) {
		na_set_index_step(q,i,size,size-1,-1);
	    } else
	    if (MATCH_INT(s)) {
		beg = NUM2SIZE(rb_str_to_inum(s,10,0));
		na_set_index_scalar(q,i,size,beg);
	    } else {
		rb_raise(rb_eIndexError,"invalid string argument '%s'",
			 StringValuePtr(s));
	    }
	    break;

        case 0:
	    // all
	    na_set_index_step(q,i,size,0,1);
	    break;

        default:
	    rb_raise(rb_eIndexError, "too many colon");
        }
	break;

    default:
        // Range object
	/*
        if (rb_obj_is_kind_of(a, rb_cRange)) {
	    step = 1;

	    beg = NUM2LONG(rb_ivar_get(a, id_beg));
	    if (beg<0)
		beg += size;

	    end = NUM2LONG(rb_ivar_get(a, id_end));
	    if (end<0)
		end += size;
	    if (RTEST(rb_funcall((a),id_exclude_end,0)))
		end--;

	    if ( beg < -size || beg >= size || 
		 end < -size || end >= size )
		rb_raise( rb_eRangeError,
			  "beg=%ld,end=%ld is out of array size (%ld)",
			  beg, end, size );
	    n = end-beg+1;
	    if (n<0) n=0;
	    na_set_index_step(q,i,n,beg,step);
        }
        // Num::Step Object
	else if (rb_obj_is_kind_of(a, num_cStep)) {*/
        if (rb_obj_is_kind_of(a, rb_cRange) || rb_obj_is_kind_of(a, num_cStep)) {

	    num_step_array_index( a, size, &n, &beg, &step );
	    /*
	    a = num_step_parameters( a, ULONG2NUM(size) );
	    beg  = NUM2LONG(RARRAY_PTR(a)[0]);
	    step = NUM2LONG(RARRAY_PTR(a)[1]);
	    n    = NUM2LONG(RARRAY_PTR(a)[2]);
	    */
	    na_set_index_step(q,i,n,beg,step);
        }
    	/*
        // NArray index
        if (NA_IsNArray(a)) {
    	GetNArray(a,na);
    	size = na_ary_to_index(na,shape,sl);
        } else
    	*/
	else
	    rb_raise(rb_eIndexError, "not allowed type");
    }
}


static size_t
na_parse_index_args(VALUE args, narray_t *na, na_index_arg_t *q, int nd)
{
    int i, j, k, l, nidx;
    size_t total=1;
    VALUE *idx;

    // args = rb_ary_new(argc,argv);
    // count_new = na_parse_index_preprocess(args, na->ndim);
    // nd = na->ndim+count_new;
    // q = ALLOC_N( na_index_arg_t, na->ndim+count_new );

    nidx = RARRAY_LEN(args);
    idx = RARRAY_PTR(args);

    for (i=j=k=0; i<nidx; i++) {
	// rest dimension
	if (idx[i]==Qfalse) {
	    for (l = nd - (nidx-1); l>0; l--) {
		na_parse_each_index( Qtrue, na->shape[k], k, &q[j] );
		if (q[j].n > 1)
		    total *= q[j].n;
		j++;
		k++;
	    }
	}
	// new dimension
	else if (SYM2ID(idx[i])==id_new) {
	    // na_set_index_step(&q[j],j,i,k+1,1,0,1);
	    na_parse_each_index( idx[i], 1, k, &q[j] );
	    j++;
	}
	// other dimention
	else {
	    na_parse_each_index( idx[i], na->shape[k], k, &q[j] );
	    if (q[j].n > 1)
		total *= q[j].n;
	    j++;
	    k++;
	}
    }
    return total;
}



VALUE
na_aref_md(int argc, VALUE *argv, VALUE self, int keep_dim)
{
    VALUE view, a, args;
    narray_t *na1, *na2;
    int i, j, nd, ndim, count_new;
    ssize_t size, k, total, size1;
    ssize_t *stride1, *stride2, *index, beg, step;
    na_index_arg_t *q;
    VALUE m;

    //int keep_dim=1;

    //puts("pass aref 1");

    GetNArray(self,na1);

    args = rb_ary_new4(argc,argv);
    count_new = na_parse_index_preprocess( args, na1->ndim );
    if (RARRAY_LEN(args)==1) {
	self = na_flatten(self);
	GetNArray(self,na1);
    }
    ndim = na1->ndim + count_new;
    q = ALLOC_N( na_index_arg_t, ndim );
    na_parse_index_args( args, na1, q, ndim );

    if (keep_dim) {
	nd = ndim;
    } else {
	for (i=nd=0; i<ndim; i++) {
	    if (q[i].n>1 || q[i].step!=0) {
		nd++;
	    }
	}
    }

    view = na_s_allocate( CLASS_OF(self) );
    na_copy_flags( self, view );
    GetNArray(view,na2);

    na_alloc_shape( na2, nd );
    //for (i=0, size=1; i<ndim; i++) {
    //	size *= na2->shape[i] = q[i].n;
    //}
    //na2->size = size;
    na2->data = na1->data;
    na2->offset = na1->offset;
    na_alloc_index(na2);

    stride1 = na_get_stride(self);
    //na_alloc_contiguous_stride(self);
    na2->stride = stride2 = ALLOC_N(ssize_t,nd);
    
    for (i=j=0; i<ndim; i++) {
	if (q[i].step!=0) {
	    stride2[j] = stride1[q[i].orig_dim];
	    j++;
	}
    }
    total = 1;
    //puts("pass aref 3");

    for (i=j=0; i<ndim; i++) {
	
	// numeric index
	if (!keep_dim && q[i].n==1 && q[i].step==0) {
	  //na2->index[j] = NULL;
	    na2->offset += stride1[q[i].orig_dim] * q[i].beg;
	    continue;
	}

	na2->shape[j] = size = q[i].n;

	if (q[i].mark != 0) {
	    m = rb_funcall(INT2FIX(1),rb_intern("<<"),1,INT2FIX(j));
	    na2->mark = rb_funcall(m,rb_intern("|"),1,na2->mark);
	}

	// array index
	if (q[i].idx != NULL) {
	    na2->index[j] = index = q[i].idx;
	    // index <- index
	    if (na1->index) {
		if (na1->index[q[i].orig_dim]) {
		    for (k=0; k<size; k++)
			index[k] = na1->index[q[i].orig_dim][index[k]];
		    goto loop_continue;
		}
	    }
	    // index <- step
	    stride2[j] = step = stride1[q[i].orig_dim];
	    size1 = na1->shape[q[i].orig_dim];
	    if (step<0) {
		step = stride2[i] = -step;
		if (na2->offset < size1 * step)
		    rb_raise(rb_eStandardError,"bug: negative offset");
		na2->offset -= size1 * step;
		for (k=0; k<size; k++)
		    index[k] = (size1 - index[k])*step;
	    } else {
		for (k=0; k<size; k++)
		    index[k] *= step;
	    }
	} else {
	    beg  = q[i].beg;
	    step = q[i].step;
	    // step <- index
	    if (na1->index) {
		if (na1->index[q[i].orig_dim]) {
		    na2->index[j] = ALLOC_N(size_t, size);
		    for (k=0; k<size; k++)
			na2->index[j][k] = na1->index[q[i].orig_dim][beg+step*k];
		    goto loop_continue;
		}
	    }
	    // step <- step
	    na2->index[j] = NULL;
	    stride2[j] = stride1[q[i].orig_dim];
	    //printf("stride2[j=%d] = %ld, step=%ld\n", j, stride2[j], step);
	    na2->offset   += stride2[j]*beg;
	    if (size==1 && step==0) step=1;
	    na2->stride[j] = stride2[j]*step;
	}
    loop_continue:
	// drop dimension
	j++;
	total *= size;
    }

    na2->size = total;
    //if (na2->ndim == 0) {
    //  char *ptr = na_data_pointer_for_read(view);
    //  view = rb_float_new(*(double*)(ptr + na2->offset));
    //}
    return view;
}


/* method: [](idx1,idx2,...,idxN) */
static VALUE
na_aref_main(int nidx, VALUE *idx, VALUE self, int keep_dim)
{
    na_index_arg_to_internal_order( nidx, idx, self );

    if (nidx==0) {
	return na_copy(self);
    }
    if (nidx==1) {
      if (CLASS_OF(*idx)==cBit) {
	rb_funcall(*idx,rb_intern("mask"),1,self);
      }
    }
    //if (nidx==1) {
    //	self = na_flatten(self);
    //}
    /*
      if ( NA_IsNArray(idx[0]) ) {
      if( NA_TYPE(idx[0]) == NA_BYTE ) // then supposed to be a mask
      return na_aref_mask(self, idx[0]);
      }
      if ( na_class_dim(CLASS_OF(self)) != 1 ) {
      if ( NA_IsArray(idx[0]) ) // Array Index ?
      return na_aref_single_dim_array( self, idx[0] );
      else
      return na_aref_single_dim( self, idx[0], flag );
      }
      }
    */
    // if (nidx>1)
    return na_aref_md( nidx, idx, self, keep_dim );
}

/* method: [](idx1,idx2,...,idxN) */
VALUE na_aref(int argc, VALUE *argv, VALUE self)
{
    VALUE view;
    view = na_aref_main(argc, argv, self, 0);
    return rb_funcall(view, rb_intern("extract"), 0);
}

/* method: slice(idx1,idx2,...,idxN) */
VALUE na_slice(int argc, VALUE *argv, VALUE self)
{
    return na_aref_main(argc, argv, self, 1);
}


#define UNITDATA ssize_t

void
na_copy_bytes( na_iterator_t *const itr, VALUE info )
{
    size_t  i, s1, s2;
    char   *p1, *p2;
    char   *q1, *q2;
    size_t *idx1, *idx2;
    int     j, e;

    INIT_COUNTER( itr, i );
    INIT_PTR( itr, 0, p1, s1, idx1 );
    INIT_PTR( itr, 1, p2, s2, idx2 );
    e = itr[0].elmsz;
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
	for (j=e; j>=sizeof(UNITDATA); j-=sizeof(UNITDATA)) {
	    *(UNITDATA*)q2 = *(UNITDATA*)q1;
	    q1 += sizeof(UNITDATA);
	    q2 += sizeof(UNITDATA);
	}
	for (; j--;) {
	    *q2++ = *q1++;
	}
    }
}


VALUE
na_copy( VALUE self )
{
    ndfunc_t *func;
    VALUE v;

    func = ndfunc_alloc( na_copy_bytes, HAS_LOOP, 1, 1, Qnil, INT2FIX(0) );
    v = ndfunc_execute( func, 1, self );
    ndfunc_free(func);
    return v;
}




VALUE
na_store( VALUE dst, VALUE src )
{
    ndfunc_t *func;
    
    func = ndfunc_alloc( na_copy_bytes, HAS_LOOP, 2, 0, INT2FIX(1), Qnil );
    ndfunc_execute( func, 2, src, dst );
    ndfunc_free(func);
    return dst;
}


/* method: []=(idx1,idx2,...,idxN,val) */
static VALUE
na_aset(int argc, VALUE *argv, VALUE self)
{
    VALUE a;
    argc--;

    if (argc==0)
	na_store( self, argv[argc] );
    else {
	a = na_aref_main( argc, argv, self, 0 );
	//a = na_aref( argc, argv, self );
	na_store( a, argv[argc] );
    }
    return argv[argc];
}


// convert mark dims to 0-th element
// for initialization of min/max func
// ['*,+,*'] -> [true,0,true]
VALUE num_init_accum_aref0( VALUE self, VALUE mark )
{
    narray_t *na;
    VALUE a;
    ID id_bra;
    unsigned long m;
    int i, ndim;

    GetNArray(self,na);
    ndim = na->ndim;
    a = rb_ary_new();
    if (FIXNUM_P(mark)) {
	m = NUM2ULONG(mark);
	if (m==0)
	    for (i=0; i<ndim; i++)
		rb_ary_push(a,INT2FIX(0));
	else
	    for (i=0; i<ndim; i++)
		if ((m>>i) & 1u)
		    rb_ary_push(a,INT2FIX(0));
		else 
		    rb_ary_push(a,Qtrue);
    } else {
	id_bra = rb_intern("[]");
	for (i=0; i<ndim; i++)
	    if (rb_funcall(mark,id_bra,1,INT2FIX(i)) == INT2FIX(1))
		rb_ary_push(a,INT2FIX(0));
	    else 
		rb_ary_push(a,Qtrue);
    }
    return na_aref_md( RARRAY_LEN(a), RARRAY_PTR(a), self, 0 );
}



void
Init_na_index()
{
    //rb_define_method(cNArray, "aref_index", na_aref_index, -1);
    rb_define_method(cNArray, "[]", na_aref, -1);
    rb_define_method(cNArray, "slice", na_slice, -1);

    rb_define_method(cNArray, "copy", na_copy, 0);
    rb_define_method(cNArray, "store", na_store, 1);

    rb_define_method(cNArray, "[]=", na_aset, -1);


    id_ast = rb_intern("*");
    id_all = rb_intern("all");
    id_minus = rb_intern("-");
    id_new = rb_intern("new");
    id_reverse = rb_intern("reverse");
    id_plus = rb_intern("+");
    id_mark = rb_intern("mark");
    id_sum = rb_intern("sum");
    id_tilde = rb_intern("~");
    id_rest = rb_intern("rest");
    id_beg = rb_intern("begin");
    id_end = rb_intern("end");
    id_exclude_end = rb_intern("exclude_end?");
}
