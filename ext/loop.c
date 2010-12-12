/*
  loop.c
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

#ifdef HAVE_STDARG_PROTOTYPES
#include <stdarg.h>
#define va_init_list(a,b) va_start(a,b)
#else
#include <varargs.h>
#define va_init_list(a,b) va_start(a)
#endif


static ndfunc_t *
//ndfunc_alloc_main( na_iter_func_t _func, int _has_loop, int _narg, int _nres, VALUE *etypes )
ndfunc_alloc_main( na_iter_func_t _func, int _flag, int _narg, int _nres, VALUE *etypes )
{
    int i, n = _narg+_nres;
    ndfunc_t *nf;
    nf = (ndfunc_t*)xmalloc( sizeof(ndfunc_t) +
			     sizeof(na_func_arg_t)*(n) );
    //nf->name = name;
    nf->func = _func;
    //nf->has_loop = _has_loop;
    //nf->keep_dim = 0;
    nf->flag = _flag;
    nf->narg = _narg;
    nf->nres = _nres;
    nf->args = (na_func_arg_t*)(nf+1);
    for (i=0; i<n; i++) {
        nf->args[i].dim  = 0;
        nf->args[i].init = Qnil;
	nf->args[i].type = etypes[i];
	nf->args[i].aux.shape_p = NULL;
    }
    return nf;
}



ndfunc_t *
#ifdef HAVE_STDARG_PROTOTYPES
ndfunc_alloc( na_iter_func_t func, int flag, int narg, int nres, ... )
#else
ndfunc_alloc( func, flag, narg, nres, va_alist )
  //  char *name;
  na_iter_func_t func;
  int flag;
  int narg;
  int nres;
  va_dcl
#endif
{
    va_list ar;
    va_init_list(ar, nres);

    VALUE *argv;
    int n = narg + nres;

    if (n > 0) {
	int i;
	argv = ALLOCA_N(VALUE, n);
	for (i=0;i<n;i++) {
	    argv[i] = va_arg(ar, VALUE);
	}
	va_end(ar);
    }
    else {
	argv = 0;
    }
    return ndfunc_alloc_main( func, flag, narg, nres, argv );
}



void
ndfunc_free( ndfunc_t* nf )
{
    int i, n;
    //puts("na_func_free called");
    n = nf->narg + nf->nres;
    for (i=0; i<n; i++)
	if (nf->args[i].dim>1)
	    if (nf->args[i].aux.shape_p)
		xfree(nf->args[i].aux.shape_p);
    //if ( nf->args )
    //	xfree(nf->args);
    //nf->func = NULL;
    //nf->has_loop = 0;
    //nf->narg = 0;
    //nf->nres = 0;
    //nf->args = NULL;
    xfree(nf);
}



VALUE num_type_s_cast(VALUE type, VALUE obj);


void ndfunc_init_md_loop(
  ndfunc_t *nf,
  VALUE *argv,
  int    max_nd,
  int    arg_nd,
  VALUE  mark,
  na_iterator_t **itr,
  VALUE results // result array
)
{
    narray_t *na;
    int i, j, jj, dim;
    int narg, nres, nall;
    int ndim;
    int dim_beg, dim_end, dim_ary;
    int dst_dim;
    size_t k;
    volatile VALUE v, t;
    size_t u_len, u_stride;
    size_t *stride, *stride_temp;
    size_t st, elmsz; // bit_size;
    size_t *shape;
    char *ptr;
    size_t loop_nd;
    size_t imark;

    stride_temp = ALLOC_N(size_t,max_nd);

    narg = nf->narg;
    nres = nf->nres;
    nall = narg+nres;

    loop_nd = max_nd - arg_nd;

    // argument arrays
    for (j=0; j<narg; j++) {
	v = argv[j];
	t = nf->args[j].type;
	//if ( FIXNUM_P(t) ) {
	//    t = CLASS_OF(argv[FIX2INT(t)]);
	//}
	//if ( rb_obj_is_kind_of( t, rb_cClass ) ) {
	//    if ( RTEST(rb_class_inherited_p( t, cNArray )) ) {
	//	if ( !RTEST(rb_class_inherited_p( CLASS_OF(v), t )) ) {
	//	    argv[j] = v = num_type_s_cast(t, v);
	//	    puts("cast");
	//	}
	//    }
	//}
	if (IsNArray(v)) {
	    if (TEST_BYTE_SWAPPED(v) && !NDF_TEST(nf,NDF_ACCEPT_SWAP)) {
	        v = rb_funcall(v, rb_intern("swap_byte"), 0);
	    }
	    GetNArray(v,na);
	    //bit_size = NUM2INT(rb_const_get( CLASS_OF(v), id_element_bit_size ));
	    elmsz = NUM2INT(rb_const_get( CLASS_OF(v), id_contiguous_stride ));
	    ptr = na_data_pointer_for_write(v);
	    stride = na_get_stride(v);

	    ndim = na->ndim - nf->args[j].dim;  // dimention used for md-loop

	    for (dim=0; dim<=max_nd; dim++) {
		itr[dim][j].n = 1;
		itr[dim][j].pos = 0;
		itr[dim][j].step = 0;
		itr[dim][j].elmsz = elmsz;
		itr[dim][j].idx = NULL;
		itr[dim][j].ptr = ptr;
	    }
	    itr[0][j].pos = na->offset;

	    dim_end = loop_nd + nf->args[j].dim;
	    dim_beg = dim_end - na->ndim;

	    for (dim=dim_end-1; dim>=dim_beg; dim--) {
		dim_ary = dim - dim_beg;
		itr[dim][j].n = na->shape[dim_ary];
		itr[dim][j].step = stride[dim_ary];
		//printf("dim=%d, j=%d, itr[dim][j].step=%ld\n", dim, j, itr[dim][j].step);
		if (na->index) {
		    itr[dim][j].idx = na->index[dim_ary];
		}
	    }
	}
	else if (TYPE(v)==T_ARRAY) {
	}
    }

    // 数チェック itr[dim][0...nargs].n が同じ、または 1
    // 繰り返し次元 if (itr[dim][j].n==1) itr[dim][j].step=0;
    // check the number of iteration for each dimension
    shape = ALLOC_N( size_t, max_nd );
    for (dim=0; dim < loop_nd ; dim++) {
	shape[dim] = 1;
    	for (j=0; j<narg; j++) {
	    if (itr[dim][j].n>1) {
		if (shape[dim]==1) {
		    shape[dim] = itr[dim][j].n;
		} else
		if (shape[dim] != itr[dim][j].n) {
		    rb_raise(rb_eTypeError,"shape1[%d](=%d) != shape2[%d](=%d)",
			     dim, shape[dim], dim, itr[dim][j].n );
		}
	    }
	}
    	for (j=0; j<narg; j++) {
	    if (shape[dim]>1 && itr[dim][j].n==1) {
		itr[dim][j].n = shape[dim];
		itr[dim][j].step = 0;
		if (itr[dim][j].idx) {
		    itr[dim][j].pos = itr[dim][j].idx[0];
		    itr[dim][j].idx = NULL;
		}
	    }
    	}
    }

    // Shape for result arrays
    //u_len = 1;
    for (dim=0; dim<max_nd; dim++) {
	shape[dim] = itr[dim][0].n;
	//u_len *= itr[dim][0].n; //n[i];
	//printf("shape[%ld]=%ld\n",dim,shape[dim]);
    }


    // result arrays
    for (j=narg; j<nall; j++) {
	t = nf->args[j].type;
	if ( FIXNUM_P(t) ) {
	    t = CLASS_OF(argv[FIX2INT(t)]);
	}
	if ( rb_obj_is_kind_of( t, rb_cClass )) {
	    if ( RTEST(rb_class_inherited_p( t, cNArray )) ) {
		// setup shape
		//printf("loop_nd=%d\n",loop_nd);
		for (dim=dst_dim=0; dim<loop_nd; dim++) {
		    //printf("mark = %lx\n", mark);
		    //printf("shape[dim=%d] = %lx\n", dim, shape[dim]);
		    if (na_test_mark(mark,dim)) { //  mark dimension
			if (FL_TEST(nf,NDF_KEEP_DIM))
			    shape[dst_dim++] = 1;
			continue;
		    }
		    shape[dst_dim++] = itr[dim][0].n;
		}
		//printf("nf->args[j=%d].dim=%d\n",j,nf->args[j].dim);
		if (nf->args[j].dim==1) {
		    shape[dst_dim++] = nf->args[j].aux.shape[0];
		} else if (nf->args[j].dim>1) {
		    for (dim=0; dim<nf->args[j].dim; dim++) {
			shape[dst_dim] = nf->args[j].aux.shape_p[dim];
			//printf("shape[dim=%d] = %lx\n", dst_dim, shape[dst_dim]);
			dst_dim++;
		    }
		}
		// in-place check
		for (jj=0; jj<narg; jj++) {
		    v = argv[jj];
		    if (TEST_INPLACE(v)) {
			// type check
			if (t != CLASS_OF(v))
			    goto not_in_place;
			// already used for result ?
			for (i=0; i<RARRAY_LEN(results); i++) {
			    if (v == RARRAY_PTR(results)[i])
				goto not_in_place;
			}
			GetNArray(v,na);
			// shape check
			if (na->ndim == dst_dim) {
			    for (i=0; i<dst_dim; i++) {
				if (shape[i] != na->shape[i])
				    goto not_in_place;
			    }
			    goto in_place;
			}
		    }
		not_in_place:
		    ;
		}
		// new object
		v = rb_narray_new( t, dst_dim, shape );
		GetNArray(v,na);
	    in_place:
		if (nf->args[j].init != Qnil) {
		    na_store( v, nf->args[j].init );
		    //num_dfloat_debug_print(v);
		    //rb_narray_debug_info(v);
		}
		//bit_size = NUM2INT(rb_funcall( t, rb_intern("bit_size"), 0 ));
		//bit_size = NUM2INT(rb_const_get( CLASS_OF(v), id_element_bit_size ));
		elmsz = NUM2INT(rb_const_get( CLASS_OF(v), id_contiguous_stride ));
		ptr = na_data_pointer_for_write(v);
		stride = na_get_stride(v);
		// set iterator
		for (dim=0; dim<=max_nd; dim++) {
		    itr[dim][j].pos = 0; //na->offset;
		    itr[dim][j].step = 0;
		    itr[dim][j].elmsz = elmsz;
		    itr[dim][j].ptr = ptr;
		    itr[dim][j].idx = NULL;
		}
		itr[0][j].pos = na->offset;
		for (dim=dst_dim=0; dst_dim<na->ndim; dim++) {
		    if ( NDF_TEST(nf,NDF_KEEP_DIM) || !na_test_mark(mark,dim) ) { // mark dimension
			if (shape[dst_dim]>1 || itr[dim][0].n==1)
			    itr[dim][j].step = stride[dst_dim];
			dst_dim++;
		    }
		}

		rb_ary_push( results, v );
		continue;
	    }
	}
	// else Array
	for (dim=0; dim<=max_nd; dim++) {
	    itr[dim][j].pos = 0;
	    itr[dim][j].step = sizeof(VALUE);
	    itr[dim][j].elmsz = sizeof(VALUE);
	    itr[dim][j].idx = NULL;
	}
    }

    //printf("itr[0][0].step=%ld\n", 0, itr[0][0].step);

    // 縮約次元   itr[dim][j].n *= itr[dim-1][j].n; itr[dim-1][j].n = 1;
    // 条件:  itr[dim-1][j].step == itr[dim][j].n * itr[dim][j].step
    //	      step==0 でもこの条件でオケ
    //	      itr[dim][j].pos == 0
    //	      itr[dim][j].idx == NULL
    //	      itr[dim-1][j].idx == NULL

    xfree(shape);
    xfree(stride_temp);
}


static int
ndfunc_enable_loop( ndfunc_t *nf, na_iterator_t *itr )
{
    int i, nargs = nf->narg + nf->nres;

    if (NDF_TEST(nf,NDF_STEP_LOOP)) {
	if (NDF_TEST(nf,NDF_INDEX_LOOP)) {
	    //puts("pass 01");
	    return 1;
	}
	for (i=0; i<nargs; i++) {
	    if (itr[i].idx) {
		return 0;
	    }
	}
	//puts("pass 02");
	return 1;
    }
    if (NDF_TEST(nf,NDF_CONTIGUOUS_LOOP)) {
	for (i=0; i<nargs; i++) {
	    if (itr[i].idx || itr[i].step != itr[i].elmsz)
		return 0;
	}
	//puts("pass 03");
	return 1;
    }
    return 0;
}


static void ndfunc_md_loop(
  ndfunc_t *nf,
  int nd,
  na_iterator_t **itr,
  VALUE info
)
{
    size_t *c;
    int  dim;
    int  j;
    int  nargs = nf->narg + nf->nres;

    //if (nf->has_loop && nd>0) nd--;
    //if (NDF_TEST(nf,NDF_HAS_LOOP)) nd--;
    if (j=ndfunc_enable_loop(nf,itr[nd-1])) nd--;
    //printf("nd = %d\n", nd);
    //printf("has_loop = %d\n", j);
    //printf("nf->flag = %d\n", nf->flag);

    if (nd<1) {
	(*(nf->func))( itr[nd], info );
	return;
    }

    // counter
    c = ALLOC_N( size_t, nd+1 );
    for (dim=0; dim<=nd; dim++) c[dim]=0;

    // loop body
    for (dim=0;; ) {
	for (; dim<nd; dim++) {
	    for (j=0; j<nargs; j++) {
		if (itr[dim][j].idx) {
		    itr[dim+1][j].pos = itr[dim][j].pos + itr[dim][j].idx[c[dim]];
		} else {
		    itr[dim+1][j].pos = itr[dim][j].pos + itr[dim][j].step*c[dim];
		}
		//printf("itr[dim+1=%d][j=%d].pos=%ld\n",dim+1,j,itr[dim+1][j].pos);
	    }
	}
	(*(nf->func))( itr[nd], info );

	for (;;) {
	    if (dim<=0) goto loop_end;
	    //printf("c[dim=%d] = %ld\n",dim-1,c[dim-1]);
	    if ( ++c[--dim] < itr[dim][0].n ) break;
	    c[dim] = 0;
	}
    }
 loop_end:
    xfree(c);
}


// a[5][4]
// a[0][0]
// a[0][1]
// ...
// step[0] = 32 step[1] = 8

// pos[1] = pos[0] + step[0]*0
//   pos[2] = pos[1] + step[1]*0
//   pos[2] = pos[1] + step[1]*1
//    ..
//   pos[2] = pos[1] + step[1]*3

// pos[1] <- 32 <- pos[0] + step[0]*1
//   pos[2] <- 32 <- pos[1] + step[1]*0
//   pos[2] <- 40 <- pos[1] + step[1]*1
//    ..
//   pos[2] <- 56 <- pos[1] + step[1]*3

// pos[1] = pos[0] = 32
// pos[0] = 32 pos[1] = 32
// pos[0] = 32 pos[1] = 40
// ..
// pos[0] = 32 pos[1] = 56
// pos[0] = 64 pos[1] = 64
//
// pos = beg + step*i
// ofs[dim+1] = ofs[dim] + step[dim] * i

VALUE na_aref(int argc, VALUE *argv, VALUE self);

VALUE ndfunc_execute_reduce_main( ndfunc_t *nf, VALUE mark, VALUE args )
{
    narray_t *na;
    int i, j;
    int ninp, nout;
    int nd, tmp_nd;
    int nargs;
    int arg_dim;
    int ndim, ni;
    volatile VALUE a, v;
    //size_t *n;
    VALUE info=Qnil;
    na_iterator_t **itr, *itra;
    int argc;
    VALUE *argv;
    VALUE ref;
    VALUE t;

    argc = RARRAY_LEN(args);
    argv = RARRAY_PTR(args);

    //GetNdfunction(self,nf);
    ninp = nf->narg;
    nout = nf->nres;
    nargs = ninp+nout;
    // check ninp+nout == argc
    if (argc == ninp+1) {
	info = argv[ninp];
    } else
    if (argc != ninp) {
	rb_raise(rb_eArgError, "wrong number of arguments (%d for %d)", argc, ninp);
    }

    // find max dimension
    arg_dim = 0;
    for (j=0; j<ninp; j++) {
	tmp_nd = nf->args[j].dim;
	if (tmp_nd > arg_dim)
	    arg_dim = tmp_nd;
    }
    nd = 0;
    for (j=0; j<ninp; j++) {
	// cast
	v = argv[j];
	t = nf->args[j].type;
	if ( FIXNUM_P(t) ) {
	    t = CLASS_OF(argv[FIX2INT(t)]);
	}
	if ( rb_obj_is_kind_of( t, rb_cClass ) ) {
	    if ( RTEST(rb_class_inherited_p( t, cNArray )) ) {
		if ( !RTEST(rb_class_inherited_p( CLASS_OF(v), t )) ) {
		    v = argv[j] = num_type_s_cast(t, v);
		}
	    }
	}
	// dimension
	if (TYPE(v)==T_DATA) {
	    //rb_narray_debug_info( argv[j] );
	    GetNArray(v,na);
	    //printf("na->ndim=%d, nf->args[j].dim=%d\n", na->ndim,nf->args[j].dim);
	    tmp_nd = na->ndim + arg_dim - nf->args[j].dim;
	    if (tmp_nd > nd)
		nd = tmp_nd;
	}
    }

    ndim = nd-arg_dim;
    if (ndim==0)
	ni = nd+2;
    else
	ni = nd+1;

    itr = ALLOC_N( na_iterator_t*, ni );
    itra = ALLOC_N( na_iterator_t, nargs*ni );
    for (i=0; i<ni; i++) {
	itr[i] = &(itra[nargs*i]);
    }
    if (ndim==0) itr++;

    a = rb_ary_new2( nargs );
    ndfunc_init_md_loop( nf, argv, nd, arg_dim, mark, itr, a );

    if (ndim==0)
	for (i=0; i<nargs; i++) {
	    itr[-1][i].pos = itr[0][i].pos;
	    itr[-1][i].ptr = itr[0][i].ptr;
	    itr[-1][i].elmsz = itr[0][i].elmsz;
	    itr[-1][i].n = 1;
	    itr[-1][i].step = 0;
	    itr[-1][i].idx = NULL;
	}

    ndfunc_md_loop( nf, ndim, itr, info );

    if (ndim==0) itr--;
    xfree(itr);
    xfree(itra);

    if (nout==0) {
	v = Qnil;
    } else if (nout==1) {
	v = RARRAY_PTR(a)[nout-1];
    } else {
	v = a;
    }
    return v;
}




VALUE
#ifdef HAVE_STDARG_PROTOTYPES
ndfunc_execute( ndfunc_t *nf, int argc, ... )
#else
ndfunc_execute( nf, argc, va_alist )
  ndfunc_t *nf;
  int argc;
  va_dcl
#endif
{
    va_list ar;
    va_init_list(ar, argc);

    volatile VALUE args;
    int i;

    args = rb_ary_new2(argc);
    for (i=0; i<argc; i++) {
	rb_ary_push( args, va_arg(ar, VALUE) );
    }
    va_end(ar);

    return ndfunc_execute_reduce_main( nf, Qnil, args );
}

VALUE
#ifdef HAVE_STDARG_PROTOTYPES
ndfunc_execute_reduce( ndfunc_t *nf, VALUE mark, int argc, ... )
#else
ndfunc_execute( nf, argc, va_alist )
  ndfunc_t *nf;
  int argc;
  va_dcl
#endif
{
    va_list ar;
    va_init_list(ar, argc);

    volatile VALUE args;
    int i;

    args = rb_ary_new2(argc);
    for (i=0; i<argc; i++) {
	rb_ary_push( args, va_arg(ar, VALUE) );
    }
    va_end(ar);

    return ndfunc_execute_reduce_main( nf, mark, args );
}


/*
VALUE nf_execute( int argc, VALUE *argv, VALUE self )
{
    ndfunc_t *nf;
    GetNdfunction(self,nf);
    return ndfunc_execute( nf, argc, argv );
}
*/




VALUE
ndfunc_md_loop_to_rarray(
  ndfunc_t *nf,
  int nd,
  na_iterator_t **itr,
  VALUE info
)
{
    size_t *c, i;
    int  dim, ld;
    int  j=0, nargs=1;
    int  has_loop;
    VALUE  *a;
    volatile VALUE ary, a0, tmp;

    if (nd==0) {
	ary = rb_ary_new2(1);
	itr[0][1].ptr = (char*)RARRAY_PTR(ary);
	(*(nf->func))( itr[0], info );
	return RARRAY_PTR(ary)[0];
    }

    ary = rb_ary_new();
    if (itr[0][0].n==0) return ary;

    //if (!NDF_TEST(nf,NDF_HAS_LOOP)) {
    if (!ndfunc_enable_loop(nf,itr[nd-1])) {
	has_loop = 0;
	ld = nd - 1;
    } else {
	has_loop = 1;
	ld = nd;
	tmp = rb_ary_new2(1);
	rb_ary_push(tmp,INT2FIX(0));
	itr[nd][1].ptr = (char*)RARRAY_PTR(tmp);
    }

    // counter
    c = ALLOC_N( size_t, nd+1 );
    for (dim=0; dim<=nd; dim++) c[dim]=0;

    a = ALLOC_N( VALUE, nd+1 );
    a[0] = a0 = ary;

    //printf("itr[0][0].step=%ld\n", itr[0][0].step);

    // loop body
    for (dim=0;; ) {
	for (; dim<ld; dim++) {
	    if (itr[dim][0].idx) {
		itr[dim+1][0].pos = itr[dim][0].pos + itr[dim][0].idx[c[dim]];
	    } else {
		itr[dim+1][0].pos = itr[dim][0].pos + itr[dim][0].step*c[dim];
	    }
	    //printf("dim=%d, j=%d, itr[dim][j].step=%ld\n", dim, j, itr[dim][j].step);
	    if (dim < nd-1) {
		a[dim+1] = ary = rb_ary_new2( itr[dim][0].n );
		rb_ary_push( a[dim], ary );
	    }
	}

	//printf("itr[dim=%d][0].step=%ld\n", dim, itr[dim][0].step);

	//if (!NDF_TEST(nf,NDF_HAS_LOOP)) {
	if (!has_loop) {
	    rb_ary_store(a[dim], itr[dim][0].n-1, Qnil);
	    itr[dim][1].ptr = (char*)RARRAY_PTR(a[dim]);
	    (*(nf->func))( itr[dim], info );
	} else {
	    (*(nf->func))( itr[dim], info );
	    //rb_ary_push( a[dim], *RARRAY_PTR(tmp) );
	    rb_ary_push( a[dim-1], *(VALUE*)itr[nd][1].ptr );
	}

	for (;;) {
	    if (dim<=0) goto loop_end;
	    //printf("c[dim=%d] = %ld\n",dim-1,c[dim-1]);
	    if ( ++c[--dim] < itr[dim][0].n ) break;
	    c[dim] = 0;
	}
    }
    loop_end:
    xfree(a);
    xfree(c);
    return a0;
}

VALUE ndfunc_execute_to_rarray( ndfunc_t *nf, VALUE arg, VALUE info )
{
    narray_t *na;
    int nd, i;
    volatile VALUE v, a;
    na_iterator_t **itr, *itra;

    GetNArray(arg,na);
    nd = na->ndim;

    itr = ALLOC_N( na_iterator_t*, nd+1 );
    itra = ALLOC_N( na_iterator_t, 2*(nd+1) );
    for (i=0; i<nd+1; i++) {
	itr[i] = &(itra[2*i]);
    }

    ndfunc_init_md_loop( nf, &arg, nd, 0, Qnil, itr, Qnil );
    //printf("itr[0][0].step=%ld\n", itr[0][0].step);

    v = ndfunc_md_loop_to_rarray( nf, nd, itr, info );

    xfree(itra);
    xfree(itr);
    return v;
}



void
//ndfunc_md_loop_from_rarray(
ndloop_loop_store_array(
  ndfunc_t *nf,
  int nd,
  na_iterator_t **itr,
  VALUE arg
)
{
    size_t *c, i;
    int  dim;
    int  j=0, nargs=1;
    VALUE  *a;

    //if (NDF_TEST(nf,NDF_HAS_LOOP)) nd--;
    if (ndfunc_enable_loop(nf,itr[nd-1])) nd--;
    //printf("nd = %ld\n", nd);

    // counter
    c = ALLOC_N( size_t, nd+1 );
    for (dim=0; dim<=nd; dim++) c[dim]=0;

    a = ALLOC_N( VALUE, nd+1 );
    a[0] = arg;

    for (dim=0; dim<=nd; dim++)
	itr[dim][0].pos = 0;

    // loop body
    for (dim=0;; ) {
	for (; dim<nd && TYPE(a[dim])==T_ARRAY; dim++) {
	    if (RARRAY_LEN(a[dim])==0) break;
	    a[dim+1] = RARRAY_PTR(a[dim])[itr[dim][0].pos];
	    if (itr[dim][1].idx) {
		itr[dim+1][1].pos = itr[dim][1].pos + itr[dim][1].idx[c[dim]];
	    } else {
		itr[dim+1][1].pos = itr[dim][1].pos + itr[dim][1].step*c[dim];
	    }
	}

	if (TYPE(a[dim]) == T_ARRAY) {
	    itr[dim][0].n   = RARRAY_LEN(a[dim]);
	    itr[dim][0].ptr = (char*)RARRAY_PTR(a[dim]);
	} else {
	    itr[dim][0].n   = 1;
	    itr[dim][0].ptr = (char*)(&a[dim]);
	}
	(*(nf->func))( itr[dim], Qnil );

	for (;;) {
	    if (dim<=0) goto loop_end;
	    dim--;
	    c[dim]++;
	    if ( ++itr[dim][0].pos < RARRAY_LEN(a[dim]) ) break;
	    itr[dim][0].pos = 0;
	    c[dim] = 0;
	}
    }
    loop_end:
    xfree(a);
    xfree(c);
}

//void ndfunc_execute_from_rarray( ndfunc_t *nf, VALUE rary, VALUE nary )
void ndloop_execute_store_array( ndfunc_t *nf, VALUE rary, VALUE nary )
{
    narray_t *na;
    int nd, i;
    volatile VALUE v, a;
    na_iterator_t **itr, *itra;
    size_t *shape;
    VALUE type;
    VALUE argv[2] = {rary,nary};

    //shape = na_mdarray_investigate(rary, &nd, &type);

    GetNArray(nary,na);
    nd = na->ndim;
    itr = ALLOC_N( na_iterator_t*, nd+1 );
    itra = ALLOC_N( na_iterator_t, 2*(nd+1) );
    for (i=0; i<nd+1; i++) {
	itr[i] = &(itra[2*i]);
    	//itr[i][0].n = 1;
	itr[i][0].pos = 0;
	itr[i][0].step = sizeof(VALUE);
	itr[i][0].idx = NULL;
    }
    for (i=0; i<nd; i++) {
    	itr[i][0].n = na->shape[i];
    }

    //a = rb_ary_new();
    //a = Qnil;
    ndfunc_init_md_loop( nf, argv, nd, 0, Qnil, itr, Qnil );
    ndloop_loop_store_array( nf, nd, itr, rary );
    //ndfunc_md_loop_from_rarray( nf, nd, itr, rary );
    //v = RARRAY(a)->ptr[0];

    xfree(itra);
    xfree(itr);
    //return v;
}



void
ndfunc_debug_print( VALUE ary, na_simple_func_t func, VALUE opt )
{
    void *ptr;
    narray_t *na;
    int i, ii, nd;
    ssize_t *c, *pos, *stride;

    GetNArray(ary,na);
    nd = na->ndim;

    ptr = na_data_pointer_for_read(ary);
    stride = na_get_stride(ary);

    for (i=0; i<nd; i++) {
	if ( na->shape[i] == 0 ) {
	    printf("(empty) [ ]\n");
	    return;
	}
    }

    c = ALLOC_N(size_t, nd);
    for(i=0;i<nd;i++) c[i]=0;

    pos = ALLOC_N(size_t, nd+1);
    pos[0] = na->offset;

    if (nd>0)
	printf("[");
    else
	printf("(scalar) ");

    for (i=0;;) {
	if (i<nd-1) {
	    for (ii=0; ii<i; ii++) printf(" ");
	    for (; ii<nd-1; ii++) printf("[");
	}
	for (; i<nd; i++) {
	    if (na->index)
		if (na->index[i]) {
		    pos[i+1] = pos[i] + na->index[i][c[i]];
		    continue;
		}
	    pos[i+1] = pos[i] + stride[i] * c[i];
	    //printf("pos[%d]=%ld\n",i,pos[i]);
	    //printf("stride[%d]=%ld\n",i,stride[i]);
	    //printf("c[%d]=%ld\n",i,c[i]);
	}
	(*func)( ptr, pos[i], opt );
	//printf("%d", (ptr[pos[i]/NB] >> (pos[i]%NB)) & 1u);
	for (;;) {
	    if (i==0) goto loop_end;
	    if ( ++c[--i] < na->shape[i] ) break;
	    printf("]");
	    c[i] = 0;
	}
	printf(", ");
	if (i<nd-1) printf("\n ");
    }
    loop_end:
    printf("\n");
    xfree(pos);
    xfree(c);
}


#define WRITE_IO(io,str)\
rb_funcall(io,rb_intern("write"),1,rb_str_new2(str))

void
ndfunc_execute_io( VALUE ary, na_simple_func_t func, VALUE io )
{
    void *ptr;
    narray_t *na;
    int i, ii, nd;
    ssize_t *c, *pos, *stride;

    GetNArray(ary,na);
    nd = na->ndim;

    ptr = na_data_pointer_for_read(ary);
    stride = na_get_stride(ary);

    for (i=0; i<nd; i++) {
	if ( na->shape[i] == 0 ) {
	    WRITE_IO(io,"(empty) [ ]\n");
	    return;
	}
    }

    c = ALLOC_N(size_t, nd);
    for(i=0;i<nd;i++) c[i]=0;

    pos = ALLOC_N(size_t, nd+1);
    pos[0] = na->offset;

    if (nd>0)
	WRITE_IO(io,"[");
    else
	WRITE_IO(io,"(scalar)");

    for (i=0;;) {
	if (i<nd-1) {
	    for (ii=0; ii<i; ii++) WRITE_IO(io," ");
	    for (; ii<nd-1; ii++) WRITE_IO(io,"[");
	}
	for (; i<nd; i++) {
	    if (na->index)
		if (na->index[i]) {
		    pos[i+1] = pos[i] + na->index[i][c[i]];
		    continue;
		}
	    pos[i+1] = pos[i] + stride[i] * c[i];
	    //printf("pos[%d]=%ld\n",i,pos[i]);
	    //printf("stride[%d]=%ld\n",i,stride[i]);
	    //printf("c[%d]=%ld\n",i,c[i]);
	}
	(*func)( ptr, pos[i], io );
	//printf("%d", (ptr[pos[i]/NB] >> (pos[i]%NB)) & 1u);
	for (;;) {
	    if (i==0) goto loop_end;
	    if ( ++c[--i] < na->shape[i] ) break;
	    WRITE_IO(io,"]");
	    c[i] = 0;
	}
	WRITE_IO(io,", ");
	if (i<nd-1) WRITE_IO(io,"\n ");
    }
    loop_end:
    WRITE_IO(io,"\n");
    xfree(pos);
    xfree(c);
}


VALUE
na_info_str( VALUE ary )
{
    int nd, i;
    char tmp[32];
    VALUE buf;
    narray_t *na;

    GetNArray(ary,na);
    nd = na->ndim;

    buf = rb_str_new2(rb_class2name(CLASS_OF(ary)));
    rb_str_cat(buf,"#shape=[",8);
    if (nd>0) {
	for (i=0;;) {
	    sprintf(tmp,"%i",na->shape[i]);
	    rb_str_cat2(buf,tmp);
	    if (++i==nd) break;
	    rb_str_cat(buf,",",1);
	}
    }
    rb_str_cat(buf,"]",1);
    return buf;
}

//#define WRITE_BUF(buf,str) rb_str_cat2(buf,str)

void
ndfunc_execute_inspect_main( VALUE ary, VALUE buf, na_text_func_t func,
			     VALUE opt, int ncol, int nrow )
{
    void *ptr;
    narray_t *na;
    int  i, ii, nd;
    ssize_t *c, *pos, *stride;
    VALUE str;
    long  len;
    int   col=0, row=0;

    GetNArray(ary,na);
    if (NIL_P(na->data)) {
	rb_str_cat(buf,"(no data)",9);
	return;
    }
    nd = na->ndim;

    ptr = na_data_pointer_for_read(ary);
    stride = na_get_stride(ary);

    for (i=0; i<nd; i++) {
	if ( na->shape[i] == 0 ) {
	    rb_str_cat(buf,"[]",2);
	    return;
	}
    }

    rb_str_cat(buf,":\n",2);

    c = ALLOC_N(size_t, nd);
    for(i=0;i<nd;i++) c[i]=0;

    pos = ALLOC_N(size_t, nd+1);
    pos[0] = na->offset;

    if (nd>0)
	rb_str_cat(buf,"[",1);
    else
	rb_str_cat(buf,"(scalar)",8);

    col = nd*2;
    for (i=0;;) {
	if (i<nd-1) {
	    for (ii=0; ii<i; ii++) rb_str_cat(buf," ",1);
	    for (; ii<nd-1; ii++) rb_str_cat(buf,"[",1);
	}
	for (; i<nd; i++) {
	    if (na->index)
		if (na->index[i]) {
		    pos[i+1] = pos[i] + na->index[i][c[i]];
		    continue;
		}
	    pos[i+1] = pos[i] + stride[i] * c[i];
	    //printf("pos[%d]=%ld\n",i,pos[i]);
	    //printf("stride[%d]=%ld\n",i,stride[i]);
	    //printf("c[%d]=%ld\n",i,c[i]);
	}
	str = (*func)( ptr, pos[i], opt );
	len = RSTRING_LEN(str) + 2;
	//printf("col=%d len=%d ncol=%d\n",col,len,ncol);
	if (ncol>0 && col+len > ncol-3) {
	    rb_str_cat(buf,"...",3);
	    c[i-1] = na->shape[i-1];
	    //c[--i] = 0;
	    //goto line_break;
	} else {
	    rb_str_append( buf, str );
	    col += len;
	}
	//printf("%d", (ptr[pos[i]/NB] >> (pos[i]%NB)) & 1u);
	for (;;) {
	    if (i==0) goto loop_end;
	    if ( ++c[--i] < na->shape[i] ) break;
	    rb_str_cat(buf,"]",1);
	    c[i] = 0;
	}
        //line_break:
	rb_str_cat(buf,", ",2);
	if (i<nd-1) {
	    rb_str_cat(buf,"\n ",2);
	    col = nd*2;
	    row++;
	    if (row==nrow) {
		rb_str_cat(buf,"...",3);
		goto loop_end;
	    }
	}
    }
    loop_end:
    //WRITE_BUF(buf,"\n");
    xfree(pos);
    xfree(c);
    return;
}


void
ndfunc_execute_inspect( VALUE ary, VALUE str, na_text_func_t func, VALUE opt )
{
    VALUE rows, cols;
    rows = rb_ivar_get(cNArray,rb_intern("inspect_rows"));
    cols = rb_ivar_get(cNArray,rb_intern("inspect_cols"));
    ndfunc_execute_inspect_main(ary,str,func,opt,NUM2INT(cols),NUM2INT(rows));
}


// #if 0
//
// // cast rule を使わない場合？？→ 引数を強制castする
//
// VALUE
// ndfunc_arg_type( VALUE a )
// {
//   VALUE type;
//   if (IsNArray(a)) {
//     type = NA_ELEM(a);
//   } else {
//     switch (TYPE(a)) {
//     case T_FIXNUM:
//     case T_BIGNUM:
//       type = rb_cInteger;
//       break;
//       // 整数の値の大きさで分類しなくてよいか？
//     default:
//       type = CLASS_OF(a);
//     }
//   }
//   return type;
// }
//
//
// VALUE
// ndfunc_execute_w_cast( ndfunc_t *func, int argc, VALUE *argv )
// {
//   int i;
//   VALUE a, a_type, f_type;
//
//   for (i=0; i<argc; i++) {
//     a_type = ndfunc_arg_type(argv[i]);
//     f_type = func->args[i].type;
//     if (a_type != f_type)
//       argv[i] = rb_funcall(f_type, rb_intern("cast"), 1, argv[i]);
//   }
//   return ndfunc_execute( func, argc, argv );
// }
//
//
//
// VALUE
// ndfunc_execute_binary( ndfunc_t *func, VALUE a1, VALUE a2 )
// {
//   VALUE args[2];
//   VALUE a1_elem, a2_elem;
//   VALUE e1, e2, e3;
//
//   a1_elem = ndfunc_arg_type(a1);
//   a2_elem = ndfunc_arg_type(a2);
//   e1 = func->args[0].type;
//   e2 = func->args[1].type;
//   e3 = func->args[2].type;
//
//   // 引数の型が関数定義と異なる場合
//   if (a1_elem!=e1 || a2_elem!=e2) {
//     // cast rule を調べる
//     // elem = NElement::CAST_RULE[[a1_elem,a2_elem]]
//     VALUE h = rb_const_get( mNElement, rb_intern("CAST_RULE") );
//     VALUE k = rb_assoc_new( a1_elem, a2_elem );
//     VALUE result_type = rb_hash_aref( h, k );
//     if (NIL_P(result_type)) {
//       rb_raise("cast rule not found");
//       return Qnil;
//     } else if (result_type != e3) {
//       // 結果型が異なる場合
//       return rb_funcall(result_type, func->name, 2, a1, a2);
//     }
//     /*
//     // 結果が DFloatの場合、引数をキャストして実行
//     a1 = rb_funcall(e1, "cast", 1, a1);
//     a2 = rb_funcall(e2, "cast", 1, a2);
//     */
//   }
//   args[0] = a1;
//   args[1] = a2;
//   //return ndfunc_execute( func, 2, args );
//   return ndfunc_execute_w_cast( func, 2, args );
// }
//
// #endif
