/*
  narray.c
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
#include "bytedata.h"
#include "narray_config.h"


/* global variables within this module */
VALUE mNum, cNArray;
VALUE num_eCastError;
VALUE num_eShapeError;
ID id_contiguous_stride;
ID id_element_bit_size;
ID id_element_byte_size;

//#define GetNArray(obj,var)  Data_Get_Struct(obj, narray_t, var)

#define FREE_SHAPE(na)							\
    { if ((na)->shape!=NULL && (na)->shape!=&((na)->size))		\
	    xfree((na)->shape); }

#define FREE_STRIDE(na)			\
    { if ((na)->stride!=NULL) {		\
	    xfree((na)->stride);	\
	    (na)->stride=NULL;		\
	}}

#define FREE_INDEX(na)				\
    { if ((na)->index!=NULL) {			\
	    int i;				\
	    for (i=0; i<(na)->ndim; i++)	\
		if ((na)->index[i]!=NULL)	\
		    xfree((na)->index[i]);	\
	    if ((na)->index!=&((na)->stride)) {	\
		xfree((na)->index);		\
		(na)->index = NULL;		\
	    }					\
	}}


static void
na_free( narray_t* na )
{
    if ( na->size > 0 ) {
	FREE_INDEX(na);
	FREE_STRIDE(na);
	FREE_SHAPE(na);
    }
    xfree(na);
}



static void
na_mark( narray_t *na )
{
    rb_gc_mark( na->data );
    rb_gc_mark( na->mark );
}


void num_robject_mark( narray_t *na );

static void
na_mark_robject( narray_t *na )
{
    rb_gc_mark( na->data );
    rb_gc_mark( na->mark );
    //num_robject_mark( na );
}


VALUE
na_s_allocate( VALUE klass )
{
    narray_t *na = ALLOC(narray_t);

    na->data = Qnil;
    na->offset = 0;
    na->size = 0;
    na->ndim = 0;
    na->shape = NULL;
    na->stride = NULL;
    na->index = NULL;
    na->mark = INT2FIX(0);
    return Data_Wrap_Struct(klass, na_mark, na_free, na);
}


void
na_index_arg_to_internal_order( int argc, VALUE *argv, VALUE self )
{
    int i,j;
    VALUE tmp;

    if (TEST_COLUMN_MAJOR(self)) {
	for (i=0,j=argc-1; i<argc/2; i++,j--) {
	    tmp = argv[i];
	    argv[i] = argv[j];
	    argv[j] = tmp;
	}
    }
}

VALUE
na_index_array_to_internal_order( VALUE args, VALUE self )
{
    int i,j;

    if (TEST_COLUMN_MAJOR(self)) {
	return rb_funcall( args, rb_intern("reverse"), 0 );
    }
    return args;
}



VALUE na_column_major_p( VALUE self )
{
    if (TEST_COLUMN_MAJOR(self))
	return Qtrue;
    else
	return Qfalse;
}

VALUE na_row_major_p( VALUE self )
{
    if (TEST_ROW_MAJOR(self))
	return Qtrue;
    else
	return Qfalse;
}



VALUE na_byte_swapped_p( VALUE self )
{
    if (TEST_BYTE_SWAPPED(self))
      return Qtrue;
    return Qfalse;
}

VALUE na_host_order_p( VALUE self )
{
    if (TEST_BYTE_SWAPPED(self))
      return Qfalse;
    return Qtrue;
}

VALUE na_clone(VALUE);

VALUE na_in_place( VALUE self )
{
    VALUE view;
    view = na_clone(self);
    SET_INPLACE(view);
    return view;
}

VALUE na_in_place_bang( VALUE self )
{
    SET_INPLACE(self);
    return self;
}

VALUE na_in_place_store( VALUE self, VALUE val )
{
    if (self==val)
	return self;
    else
	return na_store( self, val );
}

VALUE na_in_place_p( VALUE self )
{
    if (TEST_INPLACE(self))
	return Qtrue;
    else
	return Qfalse;
}

VALUE na_out_of_place_bang( VALUE self )
{
    UNSET_INPLACE(self);
    return self;
}

 /*
order_to_internal: initialize reshape [] slice
order_to_external: shape each_with_index
position_to_int: transpose newdim sum avg .. min max sort mean
 */

void
na_to_internal_shape( VALUE self, VALUE idxargs, size_t *shape )
{
    size_t i, n, c, s;
    VALUE *ptr;

    n = RARRAY_LEN(idxargs);
    ptr = RARRAY_PTR(idxargs);

    if (!NIL_P(self) && TEST_COLUMN_MAJOR(self)) {
	c = n-1;
	s = -1;
    } else {
	c = 0;
	s = 1;
    }
    for (i=0; i<n; i++) {
	if (RTEST(rb_funcall( ptr[i], rb_intern("<"), 1, INT2FIX(0) ))) {
	    rb_raise(rb_eArgError,"size must be non-negative");
	}
	shape[c] = NUM2SIZE(ptr[i]);
	c += s;
    }
}



static const size_t zero=0;

void
na_alloc_shape( narray_t *na, int ndim )
{
    int i;

    FREE_SHAPE(na);
    na->ndim = ndim;
    na->size = 0;
    if ( ndim == 0 ) {
	na->shape = NULL;
    }
    else if ( ndim == 1 ) {
	na->shape = &(na->size);
    }
    else if ( ndim > 1 && ndim <= 1024 ) {
	na->shape = ALLOC_N( size_t, ndim );
	for (i=0; i<ndim; i++) {
	    na->shape[i] = 0;
	}
    } else {
	rb_raise(rb_eRuntimeError,"negative or too large number of dimensions?");
    }
}


static void
na_setup_shape( narray_t *na, int ndim, size_t *shape )
{
    int i;
    size_t size;

    na_alloc_shape( na, ndim );

    for (i=0, size=1; i<ndim; i++) {
	//if (shape[i]<0)
	//    rb_raise(rb_eRuntimeError,"non-positive number in shape");
	na->shape[i] = shape[i];
	size *= shape[i];
    }
    na->size = size;
}


static void
na_setup_stride( narray_t *na )
{
    int i;
    size_t st;

    FREE_STRIDE(na);
    na->stride = NULL;
}


ssize_t *
na_get_stride(VALUE v)
{
    ssize_t *stride, st;
    narray_t *na;
    int i;

    GetNArray(v,na);

    if (!na->stride) {
	na->stride = stride = ALLOC_N(ssize_t,na->ndim);
	//st = NUM2SIZE(rb_funcall( CLASS_OF(v), rb_intern("step_unit"), 0 ));
	st = NUM2SIZE(rb_const_get( CLASS_OF(v), id_contiguous_stride ));
	//printf("step_unit=%ld, CLASS_OF(v)=%lx\n",st, CLASS_OF(v));
	for (i=na->ndim; i>0; ) {
	    stride[--i] = st;
	    st *= na->shape[i];
	}
    }
    return na->stride;
}


void
na_alloc_index( narray_t *na )
{
    int i;

    if (na->index!=NULL) {
	for (i=0; i<na->ndim; i++)
	    if (na->index[i]!=NULL)
		xfree(na->index[i]);
	xfree(na->index);
    }

    if ( na->ndim == 0 ) {
	na->index = NULL;
    }
    else {
	na->index = ALLOC_N(ssize_t*,na->ndim);
	for (i=0; i<na->ndim; i++) {
	    na->index[i] = NULL;
	}
    }
}



void
na_setup( VALUE self, int ndim, size_t *shape )
{
    narray_t *na;
    int     i;
    size_t  size, elmsz, bytesz;
    VALUE velmsz;

    GetNArray(self,na);
    // Shape
    na_setup_shape( na, ndim, shape );
    na->offset = 0;
}


static VALUE
na_clear_data( VALUE self )
{
    narray_t *na;
    GetNArray(self,na);
    na->data = Qnil;
    return Qnil;
}

static VALUE
na_data_allocated( VALUE self )
{
    narray_t *na;
    GetNArray(self,na);
    return na->data==Qnil ? Qfalse : Qtrue;
}


static size_t
na_bytesize( VALUE self )
{
    size_t bitsz, bytesz;
    narray_t *na;
    GetNArray(self,na);

    //vbitsz = rb_funcall( CLASS_OF(self), rb_intern("bit_size"), 0 );
    bitsz = NUM2SIZE(rb_const_get( CLASS_OF(self), id_element_bit_size ));
    if (bitsz<8) {
	bytesz = (bitsz * na->size - 1) / 8 + 1;
    } else {
	bytesz = (bitsz - 1) / 8 + 1;
	bytesz *= na->size;
    }
    return bytesz;
}

void
na_alloc_data( VALUE self )
{
    size_t bytesz;
    narray_t *na;
    GetNArray(self,na);

    bytesz = na_bytesize( self );
    na->data = rb_bd_new( bytesz );
    na->offset = 0;
}

static void
na_assign_data( VALUE self, VALUE data, size_t offset )
{
    size_t bytesz, datasz;
    narray_t *na;
    GetNArray(self,na);

    bytesz = na_bytesize( self );
    datasz = NUM2SIZE(rb_funcall( data, rb_intern("size"), 0 ));
    if ( offset + bytesz > datasz ) {
	rb_raise(rb_eStandardError,"too small bytedata for this array");
    }
    na->data = data;
    na->offset = offset;
}



/*
 *  call-seq:
 *     Num::NArray.new( datatype, shape )    => narray
 *     Num::NArray.new( datatype, size )    => narray
 *
 *  Constructs a narray using the given <i>datatype</i> and <i>shape</i> or .
 *  <i>size</i>.  If the second argument is an integer, returns 1-d array.
 */

static VALUE
na_initialize( int argc, VALUE *argv, VALUE self )
{
    VALUE vtype;
    VALUE vshape;
    VALUE v, data=Qnil;
    VALUE inplace=Qfalse;
    VALUE column_major=Qfalse;
    VALUE byte_swapped=Qfalse;
    VALUE offset=INT2FIX(0);
    size_t *shape=NULL;
    size_t size;
    int ndim;
    int i;

    SET_ROW_MAJOR(self);

    for (i=0; i<argc; i++) {
	v = argv[i];

	// check shape
	if (rb_obj_is_kind_of(v,rb_cNumeric)) {
	    if (shape) {
		rb_raise(rb_eArgError,
			 "multiple numeric: nd-shape should be array of integer");
	    }
	    if (RTEST(rb_funcall( v, rb_intern("<"), 1, INT2FIX(0) ))) {
		rb_raise(rb_eArgError,"size must be non-negative");
	    }
	    size = NUM2SIZE(v);
	    ndim = 1;
	    shape = &size;
	} else if (TYPE(v) == T_ARRAY) {
	    if (shape) {
		rb_raise(rb_eArgError,
			 "multiple shape: nd-shape should be array of integer");
	    }
	    ndim = RARRAY_LEN(v);
	    if ( ndim > 128 ) {
		rb_raise(rb_eArgError,"too large number of dimensions");
	    }
	    if ( ndim > 0 ) {
		shape = ALLOCA_N( size_t, ndim );
		// setup size_t shape[] from VALUE shape argument
		na_to_internal_shape( self, v, shape );
	    } // shape=NULL if ndim == 0
	} else if (rb_obj_is_kind_of(v,cByteData)) {
	    if (data != Qnil)
		rb_raise(rb_eArgError, "multiple data");
	    data = v;
	} else if (TYPE(v) == T_HASH) {
	    rb_scan_kw_args( v,
			     "offset", &offset,
			     "inplace", &inplace,
			     "column_major", &column_major,
			     "byte_swapped", &byte_swapped,
			     NULL );
	}

    }
    na_setup( self, ndim, shape );
    if (data!=Qnil) {
	na_assign_data( self, data, NUM2SIZE(offset) );
    }
    return self;
}




VALUE
rb_narray_new( VALUE klass, int ndim, size_t *shape )
{
    volatile VALUE obj;

    obj = na_s_allocate( klass );
    na_setup( obj, ndim, shape );
    return obj;
}


VALUE
rb_narray_new_w_class( int ndim, size_t *shape, VALUE klass )
{
    volatile VALUE obj;

    obj = na_s_allocate( klass );
    na_setup( obj, ndim, shape );
    return obj;
}


void
na_copy_flags( VALUE src, VALUE dst )
{
   RBASIC(dst)->flags |= (RBASIC(src)->flags) &
	(FL_USER1|FL_USER2|FL_USER3|FL_USER4|FL_USER5|FL_USER6|FL_USER7);
}


/*
VALUE
rb_narray_view( VALUE self )
{
    volatile VALUE view;
    narray_t *na1, *na2;

    GetNArray(self,na1);

    view = na_s_allocate( CLASS_OF(self) );
    GetNArray(view,na2);
    na_setup_shape( na2, na1->ndim, na1->shape );
    na_setup_stride( na2 );
    na2->data  = na1->data;
    na2->offset = na1->offset;
    na_copy_flags( self, view );
    return view;
}

VALUE
rb_narray_single_dim_view( VALUE self )
{
    volatile VALUE view;
    narray_t *na1, *na2;

    GetNArray(self,na1);

    view = na_s_allocate( CLASS_OF(self) );
    GetNArray(view,na2);
    na_setup_shape( na2, 1, &(na1->size) );
    na_setup_stride( na2 );
    na2->data  = na1->data;
    na2->offset = na1->offset;
    na_copy_flags( self, view );
    return view;
}
*/


void
na_copy_array_structure( VALUE self, VALUE view )
{
    narray_t *na1, *na2;
    int i, ndim;
    size_t j;

    GetNArray(self,na1);
    GetNArray(view,na2);

    ndim = na1->ndim;
    na_setup_shape( na2, ndim, na1->shape );
    na_setup_stride( na2 );
    na2->data = na1->data;
    na2->offset = na1->offset;

    for (i=0; i<ndim; i++) {
	na2->shape[i] = na1->shape[i];
    }
    if (na1->stride) {
	na2->stride = ALLOC_N(ssize_t,ndim);
	for (i=0; i<ndim; i++)
	    na2->stride[i] = na1->stride[i];
    }
    if (na1->index) {
	na2->index = ALLOC_N(ssize_t*,ndim);
	for (i=0; i<ndim; i++)
	    if (na1->index[i]) {
		na2->index[i] = ALLOC_N(ssize_t,na1->shape[i]);
		for (j=0; j<na1->shape[i]; j++)
		    na2->index[i][j] = na1->index[i][j];
	    } else {
		na2->index[i] = NULL;
	    }
    }
}

VALUE
na_dup( VALUE self )
{
    VALUE view;

    view = rb_obj_dup(self);
    na_copy_flags( self, view );
    na_copy_array_structure( self, view );
    return view;
}


VALUE
na_clone( VALUE self )
{
    VALUE view;

    view = rb_obj_clone(self);
    na_copy_flags( self, view );
    //puts("!!! pass");
    na_copy_array_structure( self, view );
    return view;
}


VALUE
rb_narray_debug_info( VALUE self )
{
    int i;
    size_t j;
    narray_t *na;
    GetNArray(self,na);

    printf("%s:\n",rb_class2name(CLASS_OF(self)));
    printf("  id     = 0x%"SZF"x\n", self );
    printf("  column_major? = %d\n", TEST_COLUMN_MAJOR(self) );
    printf("  byte_swapped? = %d\n", TEST_BYTE_SWAPPED(self) );
    printf("  inplace?      = %d\n", TEST_INPLACE(self) );

    printf("  size   = %"SZF"d\n", na->size   );
    printf("  ndim   = %d\n", na->ndim  );
    printf("  shape  = 0x%"SZF"x\n", na->shape );
    printf("  stride = 0x%"SZF"x\n", na->stride);
    printf("  index  = 0x%"SZF"x\n", na->index );
    printf("  data   = 0x%"SZF"x\n", na->data  );
    printf("  offset = %"SZF"d\n", na->offset );

    if (na->shape) {
	printf("  shape = [");
	for (i=0;i<na->ndim;i++)
	    printf(" %"SZF"d", na->shape[i] );
	printf(" ]\n");
    }
    if (na->stride) {
	printf("  stride = [");
	for (i=0;i<na->ndim;i++)
	    printf(" %"SZF"d", na->stride[i] );
	printf(" ]\n");
    }
    if (na->index) {
	for (i=0;i<na->ndim;i++) {
	    if (na->index[i]) {
		printf("  index[%d] = [", i );
		for (j=0;j<na->shape[i];j++)
		    printf(" %"SZF"d", na->index[i][j] );
		printf(" ]\n");
	    }
	}
    }
    return Qnil;
}




void *
na_data_pointer_for_write( VALUE self )
{
    narray_t *na;
    GetNArray(self,na);

    if (na->size==0)
	return NULL;
    if (na->data==Qnil) {
	na_alloc_data( self );
    }
    return rb_bd_pointer_for_write( na->data );// + na->offset;
}


void *
na_data_pointer_for_read( VALUE self )
{
    narray_t *na;
    GetNArray(self,na);

    if (na->size==0)
	return NULL;
    if (na->data==Qnil) {
	na_alloc_data( self );
	//rb_funcall( self, rb_intern("fill_zero"), 1,  );
    }
    return rb_bd_pointer_for_read( na->data );// + na->offset;
}



/* method: size() -- returns the total number of typeents */
static VALUE
na_size(VALUE self)
{
    narray_t *na;
    GetNArray(self,na);
    return SIZE2NUM(na->size);
}


/* method: size() -- returns the total number of typeents */
static VALUE
na_ndim(VALUE self)
{
    narray_t *na;
    GetNArray(self,na);
    return INT2NUM(na->ndim);
}


/* method: data() -- returns binary data object */
static VALUE
na_data(VALUE self)
{
    narray_t *na;
    GetNArray(self,na);
    return na->data;
}


VALUE na_check_contiguous(VALUE self, int);


/* method: pack() -- returns packed string */
static VALUE
na_pack(VALUE self)
{
    if (na_check_contiguous(self,0))
	return rb_funcall(na_data(self),rb_intern("to_str"),0);
    else
	return na_pack(na_copy(self));
}




/* method: shape() -- returns shape, array of the size of dimensions */
static VALUE
 na_shape(VALUE self)
{
    volatile VALUE v;
    narray_t *na;
    size_t i, n, c, s;

    GetNArray(self,na);
    n = na->ndim;
    if (TEST_COLUMN_MAJOR(self)) {
	c = n-1;
	s = -1;
    } else {
	c = 0;
	s = 1;
    }
    v = rb_ary_new2(n);
    for (i=0; i<n; i++) {
	rb_ary_push( v, SIZE2NUM(na->shape[c]) );
	//RARRAY_PTR(v)[i] = SIZE2NUM(na->shape[c]);
	//RARRAY_LEN(v)++;
	c += s;
    }
    return v;
}





VALUE
na_check_ladder(VALUE self, int start_dim)
{
    narray_t *na;
    int i;

    GetNArray(self,na);
    if (start_dim<-na->ndim || start_dim>=na->ndim)
	rb_raise(rb_eRuntimeError,"start_dim out of range");
    if (start_dim<0) start_dim += na->ndim;

    if (na->index) {
	for (i=start_dim; i<na->ndim; i++) {
	    if (na->index[i])
		return Qfalse;
	}
    }
    if (na->stride==NULL) {
	return Qtrue;
    }
    for (i=na->ndim-1; i>start_dim; i--) {
	if ( na->stride[i-1] != na->stride[i]*na->shape[i] )
	    return Qfalse;
    }
    return Qtrue;
}


VALUE
na_check_contiguous(VALUE self, int start_dim)
{
    narray_t *na;
    size_t step_unit;

    GetNArray(self,na);
    //vstep_unit = rb_funcall( CLASS_OF(self), rb_intern("step_unit"), 0 );
    step_unit = NUM2SIZE(rb_const_get( CLASS_OF(self), id_contiguous_stride ));
    if (na->stride)
	if (na->stride[na->ndim-1] != step_unit) return Qfalse;
    return na_check_ladder(self, start_dim);
}


#if 0
VALUE
na_flatten(VALUE self)
{
    VALUE view;
    narray_t *na1, *na2;
    int nd, i, j;
    ssize_t *c, *pos, *index, *stride1, *index1;

    GetNArray(self,na1);
    nd = na1->ndim;

    if (nd<2) {
	return na_dup(self);
    }

    //view = rb_narray_single_dim_view(self);
    view = rb_obj_dup(self);
    na_copy_flags( self, view );
    GetNArray(view,na2);
    na_setup_shape( na2, 1, &(na1->size) );
    na_setup_stride( na2 );
    na2->data  = na1->data;
    na2->offset = na1->offset;
    if (RTEST(na_check_ladder(self,0))) {
	return view;
    }

    // for non-contiguous arrays
    // index mapping
    stride1 = na_get_stride(self);
    index = ALLOC_N(ssize_t, na1->size);

    c = ALLOC_N(size_t, nd);
    for(i=0;i<nd;i++) c[i]=0;

    pos = ALLOC_N(size_t, nd+1);
    pos[0] = 0;

    for (i=j=0;;) {
	for (; i<nd; i++) {
	    if (na1->index)
		if (na1->index[i]) {
		    pos[i+1] = pos[i] + na1->index[i][c[i]];
		    continue;
		}
	    pos[i+1] = pos[i] + stride1[i] * c[i];
	}
	index[j++] = pos[i];
	for (;;) {
	    if (i==0) goto loop_end;
	    if ( ++c[--i] < na1->shape[i] ) break;
	    c[i] = 0;
	}
    }
    loop_end:
    xfree(pos);
    xfree(c);

    GetNArray(view,na2);
    na2->index = ALLOC_N(ssize_t*,1);
    na2->index[0] = index;

    return view;
}
#endif


VALUE
na_flatten_dim(VALUE self, int sd)
{
    VALUE view;
    narray_t *na1, *na2;
    int i, j, nd, fd;
    ssize_t *c, *pos, *index, *stride1;
    size_t fd_size=1;

    GetNArray(self,na1);
    stride1 = na_get_stride(self);
    nd = na1->ndim;

    if (sd<0 || sd>=nd) {
	rb_raise(rb_eRuntimeError,"start_dim is out of range");
    }

    view = na_dup(self);
    //rb_funcall(self,rb_intern("debug_info"),0);
    //rb_funcall(view,rb_intern("debug_info"),0);

    if (nd<2 || nd==sd+1) {
	view;
    }

    GetNArray(view,na2);
    //printf("na2->ndim=%d\n",na2->ndim);
    //i=2;
    //printf("i=%d, na2->shape[i]=%ld\n", i, fd_size, na2->shape[i]);
    fd_size = 1;
    for (i=sd; i < nd; i++) {
	fd_size *= na2->shape[i];
	//printf("i=%d, fd_size=%ld na2->shape[i]=%ld\n", i, fd_size, na2->shape[i]);
    }
    na2->shape[sd] = fd_size;
    na2->stride[sd] = stride1[nd-1];
    na2->ndim = sd+1;

    if (RTEST(na_check_ladder(self,sd))) {
	return view;
    }

    // for non-contiguous arrays
    // index mapping
    //stride1 = na_get_stride(self);
    index = ALLOC_N(ssize_t, fd_size);

    fd = nd-sd;

    c = ALLOC_N(size_t, fd);
    for (i=0; i<fd; i++) c[i]=0;

    pos = ALLOC_N(size_t, fd+1);
    pos[0] = 0;

    for (i=j=0;;) {
	for (; i<fd; i++) {
	    if (na1->index)
		if (na1->index[i+sd]) {
		    pos[i+1] = pos[i] + na1->index[i+sd][c[i]];
		    continue;
		}
	    pos[i+1] = pos[i] + stride1[i+sd] * c[i];
	}
	index[j++] = pos[i];
	for (;;) {
	    if (i==0) goto loop_end;
	    if ( ++c[--i] < na1->shape[i+sd] ) break;
	    c[i] = 0;
	}
    }
    loop_end:
    xfree(pos);
    xfree(c);

    if (!na2->index) {
	na2->index = ALLOC_N(ssize_t*,sd+1);
	for (i=0; i<sd; i++)
	    na2->index[i] = NULL;
    }
    na2->index[sd] = index;

    return view;
}

VALUE
na_flatten(VALUE self)
{
    return na_flatten_dim(self,0);
}


VALUE
 na_flatten_by_mark( int argc, VALUE *argv, VALUE self )
{
    size_t  sz_mark=1;
    int     i, j, ndim;
    int     nd_mark=0, nd_rest=0;
    int    *dim_mark, *dim_rest;
    int    *map;
    VALUE   mark, view;
    narray_t *na;

    //puts("pass1");
    mark = na_mark_dimension(argc, argv, self);

    //puts("pass2");

    if (mark==INT2FIX(0)) {
	//puts("pass flatten");
	view = na_flatten_dim(self,0);
    } else {
	GetNArray(self,na);
	ndim = na->ndim;
	if (ndim==0) {
	    rb_raise(rb_eStandardError,"cannot flatten scalar(dim-0 array)");
	    return Qnil;
	}
	map = ALLOC_N(int,ndim);
	dim_mark = ALLOC_N(int,ndim);
	dim_rest = ALLOC_N(int,ndim);
	for (i=0; i<ndim; i++) {
	    if (na_test_mark( mark, i )) {
		sz_mark *= na->shape[i];
		//printf("i=%d, nd_mark=%d, na->shape[i]=%ld\n", i, nd_mark, na->shape[i]);
		dim_mark[nd_mark++] = i;
	    } else {
		//shape[nd_rest] = na->shape[i];
		//sz_rest *= na->shape[i];
		//printf("i=%d, nd_rest=%d, na->shape[i]=%ld\n", i, nd_rest, na->shape[i]);
		dim_rest[nd_rest++] = i;
	    }
	}
	for (i=0; i<nd_rest; i++) {
	    map[i] = dim_rest[i];
	    //printf("dim_rest[i=%d]=%d\n",i,dim_rest[i]);
	    //printf("map[i=%d]=%d\n",i,map[i]);
	}
	for (j=0; j<nd_mark; j++,i++) {
	    map[i] = dim_mark[j];
	    //printf("dim_mark[j=%d]=%d\n",j,dim_mark[j]);
	    //printf("map[i=%d]=%d\n",i,map[i]);
	}
	xfree(dim_mark);
	xfree(dim_rest);
	//puts("pass transpose_map");
	view = na_transpose_map(self,map);
	xfree(map);
	//rb_funcall(view,rb_intern("debug_print"),0);

	//puts("pass flatten_dim");
	view = na_flatten_dim(view,nd_rest);
	//rb_funcall(view,rb_intern("debug_print"),0);
    }
    return view;
}

VALUE
na_transpose_map(VALUE self, int *map)
{
    int  i, ndim;
    ssize_t  *stride, *stride2, **index;
    VALUE     view;
    narray_t *na1, *na2;

    GetNArray(self,na1);

    view = na_dup( self );
    GetNArray(view,na2);
    ndim = na2->ndim;
    stride2 = na_get_stride(view);
    stride  = ALLOC_N(ssize_t,ndim);
    for (i=0; i<ndim; i++)
	stride[i] = stride2[i];
    for (i=0; i<ndim; i++) {
	na2->shape[i] = na1->shape[map[i]];
	na2->stride[i] = stride[map[i]];
	//printf("i=%d, na2->shape[i]=%ld, map[i]=%d\n", i, na2->shape[i], map[i]);
    }
    xfree(stride);
    if (na2->index) {
	index = ALLOC_N(ssize_t*,ndim);
	for (i=0; i<ndim; i++)
	    index[i] = na2->index[i];
	for (i=0; i<ndim; i++)
	    na2->index[i] = index[map[i]];
	xfree(index);
    }
    return view;
}


#define SWAP(a,b,tmp) {tmp=a;a=b;b=tmp;}

VALUE
na_transpose(int argc, VALUE *argv, VALUE self)
{
    int ndim, *map, tmp;
    int row_major;
    int i, j, c, r;
    size_t len;
    ssize_t beg, step;
    VALUE   v, view;
    narray_t *na1;

    GetNArray(self,na1);
    ndim = na1->ndim;
    row_major = TEST_COLUMN_MAJOR( self );

    map = ALLOC_N(int,ndim);
    for (i=0;i<ndim;i++) {
	map[i] = i;
    }
    if (argc==0) {
	SWAP( map[ndim-1], map[ndim-2], tmp );
	goto new_object;
    }
    if (argc==2) {
	if (TYPE(argv[0])==T_FIXNUM && TYPE(argv[1])==T_FIXNUM) {
	    i = FIX2INT(argv[0]);
	    j = FIX2INT(argv[1]);
	    if (row_major) {
		i = ndim-1-i;
		j = ndim-1-j;
	    }
	    SWAP( map[i], map[j], tmp );
	    goto new_object;
	}
    }
    for (i=argc,c=ndim-1; i; ) {
	v = argv[--i];
	if (TYPE(v)==T_FIXNUM) {
	    beg = FIX2INT(v);
	    len = 1;
	    step = 0;
	} else if (rb_obj_is_kind_of(v,rb_cRange) || rb_obj_is_kind_of(v,num_cStep)) {
	    num_step_array_index( v, ndim, &len, &beg, &step );
	}
	for (j=len; j; ) {
	    r = beg + step*(--j);
	    if (row_major)
		r = ndim-1-r;
	    if ( c < 0 )
		rb_raise(rb_eArgError, "too many dims");
	    map[c--] = r;
	    //printf("r=%d\n",r);
	}
    }

 new_object:
 //   view = na_dup( self );
 //   stride = na_get_stride(view);
 //   GetNArray(view,na2);
 //   for (i=0; i<ndim; i++) {
 //	na2->shape[i] = na1->shape[map[i]];
 //	na2->stride[i] = stride[map[i]];
 //   }
 //   if (na2->index) {
 //	index = ALLOC_N(ssize_t*,ndim);
 //	for (i=0; i<ndim; i++)
 //	    index[i] = na2->index[i];
 //	for (i=0; i<ndim; i++)
 //	    na2->index[i] = index[map[i]];
 //	xfree(index);
 //   }
    view = na_transpose_map(self,map);
    xfree(map);
    return view;
}



boolean
na_test_mark( VALUE mark, int dim )
{
    size_t m;

    if (!RTEST(mark))
	return 0;
    if (FIXNUM_P(mark)) {
	m = FIX2LONG(mark);
	if ( m==0 ) return 1;
	return (m & (1u<<dim)) ? 1 : 0;
    } else {
	return ( rb_funcall(mark,rb_intern("[]"),1,INT2FIX(dim)) == INT2FIX(1) ) ?
	    1 : 0 ;
    }
}


VALUE
na_mark_dimension(int argc, VALUE *argv, VALUE self)
{
    int ndim;
    int row_major;
    int i, j, r;
    size_t len;
    ssize_t beg, step;
    VALUE  v, mark;
    narray_t *na;
    size_t m;

    GetNArray(self,na);
    ndim = na->ndim;
    row_major = TEST_COLUMN_MAJOR( self );

    if (argc==0) {
	//puts("pass argc=0");
	return na->mark;
    }

    //printf("argc=%d\n",argc);

    m = 0;
    mark = Qnil;
    for (i=0; i<argc; i++) {
	v = argv[i];
	if (TYPE(v)==T_FIXNUM) {
	    beg = FIX2INT(v);
	    if (beg<0) beg+=ndim;
	    len = 1;
	    step = 0;
	} else if (rb_obj_is_kind_of(v,rb_cRange) || rb_obj_is_kind_of(v,num_cStep)) {
	    num_step_array_index( v, ndim, &len, &beg, &step );
	} else {
	    rb_raise(rb_eTypeError, "invalid dimension argument %s", rb_obj_classname(v));
	}
	for (j=0; j<len; j++) {
	    r = beg + step*j;
	    if (row_major)
		r = ndim-1-r;
	    if ( mark==Qnil )
		if ( r < sizeof(size_t) ) {
		    m |= ((size_t)1) << r;
		    continue;
		} else {
		    mark = SIZE2NUM(m);
		}
	    v = rb_funcall( INT2FIX(1), rb_intern("<<"), 1, INT2FIX(r) );
	    mark = rb_funcall( mark, rb_intern("|"), 1, v );
	}
    }
    if (mark==Qnil) mark = SIZE2NUM(m);
    return mark;
}



static void
na_reshape_self(int argc, VALUE *argv, VALUE self)
{
    size_t *shape, total;
    ssize_t *stride, st;
    int  i, unfixed=-1;
    narray_t *na;

    GetNArray(self,na);

    if (na->size==0)
	rb_raise(rb_eRuntimeError, "cannot reshape empty array");

    if (!RTEST(na_check_ladder(self,0)))
	rb_raise(rb_eRuntimeError, "reshape only contiguous or ladder arrays");

    na_index_arg_to_internal_order( argc, argv, self );

    // get shape from argument
    shape = ALLOC_N(size_t,argc);
    for (i=0,total=1; i<argc; i++)
	switch(TYPE(argv[i])) {
	case T_FIXNUM:
	case T_BIGNUM:
	    total *= shape[i] = NUM2SIZE(argv[i]);
	    break;
	case T_TRUE:
	    unfixed = i;
	    break;
	default:
	    rb_raise(rb_eArgError,"illegal type");
	}

    if (unfixed>=0) {
	if (na->size % total != 0)
	    rb_raise(rb_eArgError, "Total size size must be divisor");
	shape[unfixed] = na->size / total;
    }
    else if (total != na->size)
	rb_raise(rb_eArgError, "Total size must be same");

    stride = na_get_stride(self);
    st = stride[na->ndim-1];

    stride = ALLOC_N(ssize_t,argc);
    for (i=argc; i>0; ) {
	stride[--i] = st;
	st *= shape[i];
    }

    // exchange data
    FREE_STRIDE(na);
    FREE_SHAPE(na);
    na->ndim = argc;
    na->shape = shape;
    na->stride = stride;
}

// method: reshape!(size1,size2,...,sizeN)
static VALUE
na_reshape_bang(int argc, VALUE *argv, VALUE self)
{
    na_reshape_self(argc, argv, self);
    return self;
}


// method: reshape(size1,size2,...,sizeN)
static VALUE
 na_reshape(int argc, VALUE *argv, VALUE self)
{
    VALUE view;
    view = na_dup(self);
    na_reshape_self(argc, argv, view);
    return view;
}

static VALUE
num_na_inspect_rows( VALUE self ) {
    return rb_ivar_get(cNArray,rb_intern("inspect_rows"));
}

static VALUE
num_na_inspect_cols( VALUE self ) {
    return rb_ivar_get(cNArray,rb_intern("inspect_cols"));
}

static VALUE
num_na_set_inspect_rows( VALUE self, VALUE rows ) {
    rb_ivar_set(cNArray,rb_intern("inspect_rows"),rows);
    return Qnil;
}

static VALUE
num_na_set_inspect_cols( VALUE self, VALUE cols ) {
    rb_ivar_set(cNArray,rb_intern("inspect_cols"),cols);
    return Qnil;
}


void Init_bytedata();
void Init_datatype();
void Init_na_array();
void Init_na_index();
void Init_num_step();

/* initialization of NArray Class */
void
Init_narray()
{
    /* require Complex class */
    // rb_require("complex");
    // cComplex = rb_const_get( rb_cObject, rb_intern("Complex") );

    /* define Num module */
    mNum = rb_define_module("Num");

    num_eCastError = rb_define_class_under(mNum, "CastError", rb_eStandardError);
    num_eShapeError = rb_define_class_under(mNum, "ShapeError", rb_eStandardError);

    /* define NArray class */
    cNArray = rb_define_class_under(mNum, "NArray", rb_cObject);

    /* Ruby allocation framework  */
    rb_define_alloc_func(cNArray, na_s_allocate);
    rb_define_method(cNArray, "initialize", na_initialize, -1);

    rb_define_method(cNArray, "size", na_size, 0);
    rb_define_alias (cNArray, "length","size");
    rb_define_alias (cNArray, "total","size");
    rb_define_method(cNArray, "data", na_data, 0);
    rb_define_method(cNArray, "shape", na_shape, 0);
    rb_define_method(cNArray, "ndim", na_ndim,0);
    rb_define_alias (cNArray, "rank","ndim");

    rb_define_method(cNArray, "in_place", na_in_place, 0);
    rb_define_alias (cNArray, "inplace","in_place");
    //rb_define_method(cNArray, "in_place=", na_in_place_store, 1);
    //rb_define_alias (cNArray, "inplace=","in_place=");
    rb_define_method(cNArray, "in_place?", na_in_place_p, 0);
    rb_define_alias (cNArray, "inplace?","in_place?");
    rb_define_method(cNArray, "in_place!", na_in_place_bang, 0);
    rb_define_alias (cNArray, "inplace!","in_place!");
    rb_define_method(cNArray, "out_of_place!", na_out_of_place_bang, 0);
    rb_define_alias (cNArray, "not_in_place!", "out_of_place!");
    rb_define_alias (cNArray, "not_inplace!", "out_of_place!");

    rb_define_method(cNArray, "column_major?", na_column_major_p, 0);
    rb_define_method(cNArray, "row_major?", na_row_major_p, 0);
    rb_define_method(cNArray, "byte_swapped?", na_byte_swapped_p, 0);
    rb_define_method(cNArray, "host_order?", na_host_order_p, 0);
#ifdef DYNAMIC_ENDIAN
#else
#ifdef WORDS_BIGENDIAN
#else // LITTLE_ENDIAN
    rb_define_alias(cNArray, "big_endian?", "byte_swapped?");
    rb_define_alias(cNArray, "network_order?", "byte_swapped?");
    rb_define_alias(cNArray, "little_endian?", "host_order?");
    rb_define_alias(cNArray, "vacs_order?", "host_order?");
#endif
#endif

    rb_define_method(cNArray, "debug_info", rb_narray_debug_info, 0);

    rb_define_method(cNArray, "dup", na_dup, 0);
    rb_define_method(cNArray, "clone", na_clone, 0);

    rb_define_method(cNArray, "pack", na_pack, 0);

    rb_define_method(cNArray, "flatten", na_flatten, 0);
    rb_define_method(cNArray, "transpose", na_transpose, -1);
    rb_define_method(cNArray, "reshape", na_reshape,-1);
    rb_define_method(cNArray, "reshape!", na_reshape_bang,-1);
    rb_define_alias(cNArray,  "shape=","reshape!");

    rb_ivar_set(cNArray, rb_intern("inspect_rows"), INT2FIX(8));
    rb_ivar_set(cNArray, rb_intern("inspect_cols"), INT2FIX(78));
    //rb_define_const(cNArray, "INSPECT_WIDTH", INT2FIX(78));
    //rb_define_const(cNArray, "INSPECT_HEIGHT", INT2FIX(4));
    rb_define_singleton_method(cNArray, "inspect_rows", num_na_inspect_rows, 0);
    rb_define_singleton_method(cNArray, "inspect_rows=", num_na_set_inspect_rows, 1);
    rb_define_singleton_method(cNArray, "inspect_cols", num_na_inspect_cols, 0);
    rb_define_singleton_method(cNArray, "inspect_cols=", num_na_set_inspect_cols, 1);

    id_contiguous_stride = rb_intern(CONTIGUOUS_STRIDE);
    id_element_bit_size = rb_intern(ELEMENT_BIT_SIZE);
    id_element_byte_size = rb_intern(ELEMENT_BYTE_SIZE);

    Init_bytedata();
    Init_datatype();
    Init_na_array();
    Init_na_index();
    Init_num_step();

    Init_na_rand();
    //Init_math();

#ifdef HAVE_LAPACK
    Init_linalg();
#endif

}
