/*
  narray.h
  Numerical Array Extention for Ruby
    (C) Copyright 1999-2003 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#ifndef NARRAY_H
#define NARRAY_H

#include <math.h>
#include "compat.h"

//#include "narray_config.h"

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#define NARRAY_VERSION "0.7.0"
#define NARRAY_VERSION_CODE 700

/*
  Data types used in NArray :
  Please modify these types if your system has any different type.
*/
#if 0
#ifndef HAVE_U_INT8_T
typedef unsigned char          u_int8_t; /* NA_BYTE */
#endif

#ifndef HAVE_INT16_T
# if SIZEOF_SHORT == 2
typedef short                  int16_t;  /* NA_SINT */
# else
---->> Please define int16_t manually because sizeof(short) != 2. <<----
# endif
#endif /* HAVE_INT16_T */

#ifndef HAVE_INT32_T
# if SIZEOF_LONG == 4
typedef long                   int32_t;  /* NA_LINT */
# else
#  if SIZEOF_INT == 4
typedef int                    int32_t;  /* NA_LINT */
#  else
---->> Please define int32_t manually because sizeof(long) != 4. <<----
#  endif
# endif
#endif /* HAVE_INT32_T */

#ifndef HAVE_U_INT32_T
# if SIZEOF_LONG == 4
typedef unsigned long                   u_int32_t;  /* NA_LINT */
# else
#  if SIZEOF_INT == 4
typedef unsigned int                    u_int32_t;  /* NA_LINT */
#  else
---->> Please define u_int32_t manually because sizeof(long) != 4. <<----
#  endif
# endif
#endif /* HAVE_U_INT32_T */

#endif // 0

#if   SIZEOF_VOIDP==SIZEOF_LONG
#define NUM2SIZE(x) NUM2ULONG(x)
#define NUM2SSIZE(x) NUM2LONG(x)
#define SIZE2NUM(x) ULONG2NUM(x)
#define SSIZE2NUM(x) LONG2NUM(x)
#define SZF "l"
#elif SIZEOF_VOIDP==SIZEOF_LONG_LONG
#define NUM2SIZE(x) NUM2ULL(x)
#define NUM2SSIZE(x) NUM2LL(x)
#define SIZE2NUM(x) ULL2NUM(x)
#define SSIZE2NUM(x) LL2NUM(x)
#define SZF "ll"
#endif

#if   SIZEOF_LONG==8
#define NUM2INT64(x) NUM2LONG(x)
#define INT642NUM(x) LONG2NUM(x)
#define NUM2UINT64(x) NUM2ULONG(x)
#define UINT642NUM(x) ULONG2NUM(x)
#define INT64FMT "l"
#elif SIZEOF_LONG_LONG==8
#define NUM2INT64(x) NUM2LL(x)
#define INT642NUM(x) LL2NUM(x)
#define NUM2UINT64(x) NUM2ULL(x)
#define UINT642NUM(x) ULL2NUM(x)
#define INT64FMT "ll"
#endif

#if   SIZEOF_LONG==4
#define NUM2INT32(x) NUM2LONG(x)
#define INT322NUM(x) LONG2NUM(x)
#define NUM2UINT32(x) NUM2ULONG(x)
#define UINT322NUM(x) ULONG2NUM(x)
#define INT32FMT "l"
#elif SIZEOF_INT==4
#define NUM2INT32(x) NUM2INT(x)
#define INT322NUM(x) UINT2NUM(x)
#define NUM2UINT32(x) NUM2UINT(x)
#define UINT322NUM(x) UINT2NUM(x)
#define INT32FMT ""
#endif



//typedef struct { float r,i; }  scomplex;
//typedef struct { double r,i; } dcomplex;


#ifndef HAVE_BOOLEAN
typedef int boolean;
#endif
#ifndef FALSE                   /* in case these macros already exist */
#define FALSE   0               /* values of boolean */
#endif
#ifndef TRUE
#define TRUE    1
#endif

typedef struct { float dat[2]; }  scomplex;
typedef struct { double dat[2]; } dcomplex;
typedef int fortran_integer;

#ifndef NARRAY_C
extern VALUE cNArray;
extern VALUE num_eCastError;
extern VALUE num_eShapeError;

//EXTERN const int na_sizeof[NA_NTYPES+1];
#endif


/* global variables within this module */
extern VALUE mNum, cNArray;
extern VALUE cBit;
extern VALUE cDFloat, cDComplex;
extern VALUE cInt32, cInt24, cInt16, cInt8;
extern VALUE cUInt32, cUInt24, cUInt16, cUInt8;
extern VALUE cInt64, cInt48;
extern VALUE cUInt64, cUInt48;
extern VALUE cRObject;
extern VALUE cPointer;
extern VALUE num_cStep;
extern VALUE cComplex;
extern ID id_contiguous_stride;
extern ID id_element_bit_size;
extern ID id_element_byte_size;
extern ID id_contiguous_stride;



//typedef struct NARRAY {
typedef struct RNArray {
    //VALUE   type;	// data type  途中ですりかわってはならない
    VALUE   data;	// byte data object 
    int     ndim;	// # of dimension 
    size_t  size;	// # of total element 
    size_t  offset;     // dataポインタから先頭データの位置へのオフセット
    	    		// : elm.unit_bits を単位とする 
    	    		// address_unit  pointer_unit access_unit data_unit
                        // elm.step_unit = elm.bit_size / elm.access_unit
                        // elm.step_unit = elm.size_bits / elm.unit_bits
    size_t  *shape;	// 各次元の要素数
    ssize_t *stride;	// 各次元におけるデータポインタの増分
    	   		// : elm.unit_bits を単位とする 
    ssize_t **index;    // インデクスアクセス配列
    VALUE   mark;
} narray_t;


/* FLAGS
   - row-major / column-major
   - Overwrite or not
   - byteswapp
   - Extensible?
   - matrix or not
*/
#define FL_NA_INPLACE     FL_USER0
#define FL_NA_INDEX_ORDER FL_USER1 // column_major/row_major 
#define FL_NA_BYTE_SWAPPED  FL_USER2 // big_endian/little_endian

#define TEST_COLUMN_MAJOR(x) FL_TEST((x),FL_NA_INDEX_ORDER)
#define TEST_ROW_MAJOR(x)    (!FL_TEST((x),FL_NA_INDEX_ORDER))
#define TEST_INPLACE(x)		FL_TEST((x),FL_NA_INPLACE)
#define TEST_BYTE_SWAPPED(x)	FL_TEST((x),FL_NA_BYTE_SWAPPED)
#define TEST_HOST_ORDER(x)	(!FL_TEST((x),FL_NA_BYTE_SWAPPED))
#define TEST_COLUMN_MAJOR(x) FL_TEST((x),FL_NA_INDEX_ORDER)

#define SET_COLUMN_MAJOR(x)     FL_SET((x),FL_NA_INDEX_ORDER)
#define SET_ROW_MAJOR(x)     FL_UNSET((x),FL_NA_INDEX_ORDER)

#define SET_INPLACE(x)     FL_SET((x),FL_NA_INPLACE)
#define UNSET_INPLACE(x)     FL_UNSET((x),FL_NA_INPLACE)

#define REVERSE_BYTE_SWAPPED(x)	FL_REVERSE((x),FL_NA_BYTE_SWAPPED)


#ifdef DYNAMIC_ENDIAN
#else
#ifdef WORDS_BIGENDIAN
#else // LITTLE_ENDIAN

#define TEST_NETWORK_ORDER(x) TEST_BYTE_SWAPPED(x)
#define TEST_VACS_ORDER(x)    TEST_HOST_ORDER(x)
#endif
#endif

/*
#define FL_NA_readonly / writable
#define FL_NA_resizable / extensible
#define FL_NA_matrix?
*/



typedef struct NA_ITERATOR {
    size_t   n;
    size_t   pos;
    size_t   step;
    size_t   elmsz;
    size_t  *idx;
    char    *ptr;
} na_iterator_t;


//typedef void (*na_iter_func_t) _((na_iterator_t *const));
typedef void (*na_iter_func_t) _((na_iterator_t *const, VALUE info));

typedef void (*na_simple_func_t) _((char*ptr, size_t pos, VALUE opt));

typedef VALUE (*na_text_func_t) _((char*ptr, size_t pos, VALUE opt));


typedef struct NA_FUNC_ARG {
    VALUE type;
    VALUE init;
    int   dim;
    union {
	size_t shape[1];
	size_t *shape_p;
    } aux;
} na_func_arg_t;

typedef struct NDFUNCTION {
    //void (*func)(na_iterator_t *const);
    //char *name;
    na_iter_func_t func;
    //boolean has_loop;
    //boolean keep_dim;
    int flag;
    int narg;
    int nres;
    na_func_arg_t *args;
    /*
    na_arg_t *ress;
    */
    //VALUE info;
} ndfunc_t;


#define NDF_HAS_LOOP		7
#define NDF_CONTIGUOUS_LOOP	(1<<0)
#define NDF_STEP_LOOP		(1<<1)
#define NDF_INDEX_LOOP		(1<<2)
#define NDF_KEEP_DIM		(1<<3)
#define NDF_ACCEPT_SWAP		(1<<4)

#define HAS_LOOP       7
#define HAS_INDEX_LOOP 7
#define HAS_STEP_LOOP  3
#define HAS_CONTIGUOUS_LOOP 1
#define NO_LOOP	       0

#define NDF_TEST(nf,fl)  ((nf)->flag&(fl))
#define NDF_SET(nf,fl)  {(nf)->flag |= (fl);}


#define GetNArray(obj,var)  Data_Get_Struct(obj, narray_t, var)
//#define GetNdfunction(obj,var)  Data_Get_Struct(obj, ndfunc_t, var)

#define IsNArray(obj) (rb_obj_is_kind_of(obj,cNArray)==Qtrue)
//#define IsNElement(obj) (rb_obj_is_kind_of(obj,cNElement)==Qtrue)
//#define IsNdfunction(obj) (rb_obj_is_kind_of(obj,cNdfunction)==Qtrue)

#define DEBUG_PRINT(v) puts(StringValueCStr(rb_funcall(v,rb_intern("inspect"),0)))


#define RNARRAY(val)		((narray_t*)DATA_PTR(val))
#define RNARRAY_TYPE(val)	(RNARRAY(val)->type)
#define RNARRAY_SIZE(val)	(RNARRAY(val)->size)
#define RNARRAY_NDIM(val)	(RNARRAY(val)->ndim)
#define RNARRAY_SHAPE(val)	(RNARRAY(val)->shape)
#define RNARRAY_OFFSET(val)	(RNARRAY(val)->offset)
#define RNARRAY_STRIDE(val)	(RNARRAY(val)->stride)
#define RNARRAY_INDEX(val)	(RNARRAY(val)->index)
#define RNARRAY_MARK(val)	(RNARRAY(val)->mark)

//#define NA_TYPE(val)	(RNARRAY(val)->type)
#define NA_SIZE(val)	(RNARRAY(val)->size)
#define NA_NDIM(val)	(RNARRAY(val)->ndim)
#define NA_SHAPE(val)	(RNARRAY(val)->shape)
#define NA_OFFSET(val)	(RNARRAY(val)->offset)
#define NA_STRIDE(val)	(RNARRAY(val)->stride)
#define NA_INDEX(val)	(RNARRAY(val)->index)
#define NA_MARK(val)	(RNARRAY(val)->mark)


#define NA_PTR(a,p)    ((a)->ptr+(p)*na_sizeof[(a)->type])
#define NA_STRUCT(val) ((narray_t*)DATA_PTR(val))
#define NA_PTR_TYPE(val,type) (type)(((narray_t*)DATA_PTR(val))->ptr)
#define NA_RANK(val)   (((narray_t*)DATA_PTR(val))->rank)
#define NA_TYPE(val)   (((narray_t*)DATA_PTR(val))->type)
#define NA_TOTAL(val)  (((narray_t*)DATA_PTR(val))->total)
#define NA_SHAPE0(val) (((narray_t*)DATA_PTR(val))->shape[0])
#define NA_SHAPE1(val) (((narray_t*)DATA_PTR(val))->shape[1])

#define NA_IsNArray(obj) \
  (rb_obj_is_kind_of(obj,cNArray)==Qtrue)
#define NA_IsArray(obj) \
  (TYPE(obj)==T_ARRAY || rb_obj_is_kind_of(obj,cNArray)==Qtrue)
#define NA_IsROBJ(d) ((d)->type==NA_ROBJ)
#define NA_IsINTEGER(a) \
  ((a)->type==NA_BYTE || (a)->type==NA_SINT || (a)->type==NA_LINT )
#define NA_IsCOMPLEX(a) \
  ((a)->type==NA_SCOMPLEX || (a)->type==NA_DCOMPLEX)
#define NA_MAX(a,b) (((a)>(b))?(a):(b))
#define NA_SWAP(a,b,tmp) {(tmp)=(a);(a)=(b);(b)=(tmp);}

#define na_class_dim(klass) NUM2INT(rb_const_get(klass, na_id_class_dim))

#define NUM2REAL(v)  NUM2DBL( rb_funcall((v),na_id_real,0) )
#define NUM2IMAG(v)  NUM2DBL( rb_funcall((v),na_id_imag,0) )

#define NA_ALLOC_SLICE(slc,nc,shp,np) \
{ slc = (struct slice*)xmalloc( sizeof(struct slice)*(nc) + \
				sizeof(int)*(np) );\
  shp = (int*)&( (slc)[nc] ); }



/* Function Prototypes */


typedef struct {
    double beg;
    double step;
    size_t count;
} seq_opt_t;

typedef struct {
    u_int64_t max;
    int64_t sign;
    int shift;
} rand_opt_t;

void rand_norm(double *a);


VALUE rb_narray_new( VALUE elem, int ndim, size_t *shape );
VALUE rb_narray_debug_info( VALUE self );

void na_copy_flags( VALUE src, VALUE dst );

void *na_data_pointer_for_write( VALUE self );
void *na_data_pointer_for_read( VALUE self );

VALUE rb_narray_single_dim_view( VALUE self );

ssize_t *na_get_stride(VALUE v);

VALUE na_mark_dimension(int argc, VALUE *argv, VALUE self);

VALUE na_transpose_map(VALUE self, int *map);

VALUE na_check_ladder(VALUE self, int start_dim);


ndfunc_t * ndfunc_alloc( na_iter_func_t func, int has_loop, int narg, int nres, ... );
void ndfunc_free( ndfunc_t* nf );

VALUE ndfunc_execute( ndfunc_t *nf, int argc, ... );
VALUE ndfunc_execute_reduce( ndfunc_t *nf, VALUE mark, int argc, ... );
VALUE ndfunc_execute_to_rarray( ndfunc_t *nf, VALUE arg, VALUE info );
//VALUE ndfunc_execute_from_rarray( ndfunc_t *nf, VALUE arg );
void ndloop_execute_store_array( ndfunc_t *nf, VALUE rary, VALUE nary );
void ndfunc_debug_print( VALUE ary, na_simple_func_t func, VALUE opt );
//void ndfunc_execute_io( VALUE ary, na_simple_func_t func, VALUE io );
//VALUE ndfunc_execute_to_text( VALUE ary, na_text_func_t func, VALUE opt );
void ndfunc_execute_inspect( VALUE ary, VALUE str, na_text_func_t func, VALUE opt );
VALUE na_info_str( VALUE ary );


VALUE na_s_allocate( VALUE klass );
void na_alloc_shape( narray_t *na, int ndim );
void na_alloc_index( narray_t *na );
void na_alloc_data( VALUE self );

VALUE na_flatten(VALUE self);
VALUE na_dup(VALUE self);
VALUE na_copy(VALUE);

VALUE na_flatten_dim(VALUE self, int sd);
VALUE na_flatten_by_mark( int argc, VALUE *argv, VALUE self );

boolean na_test_mark( VALUE mark, int dim );

int na_parse_dimension(int argc, VALUE *argv, int dimc, int *dimv, int flag);


size_t *na_mdarray_investigate(VALUE ary, int *ndim, VALUE *type);


void num_step_array_index( VALUE self, size_t ary_size, size_t *plen, ssize_t *pbeg, ssize_t *pstep );
void num_step_sequence( VALUE self, size_t *plen, double *pbeg, double *pstep );

VALUE num_type_s_upcast(VALUE type1, VALUE type2);

void na_copy_bytes( na_iterator_t *const itr, VALUE info );

void rb_scan_kw_args __((VALUE, ...));

VALUE na_aref(int argc, VALUE *argv, VALUE self);
VALUE na_aref_md(int argc, VALUE *argv, VALUE self, int keep_dim);
VALUE num_init_accum_aref0( VALUE self, VALUE mark );

int n_bits(u_int64_t a);

VALUE num_pointer_new( VALUE a1 );
VALUE num_pointer_index( VALUE ptr, void *beg, ssize_t step, VALUE idxclass );


typedef unsigned int BIT_DIGIT;
//#define BYTE_BIT_DIGIT sizeof(BIT_DIGIT)
#define NB     (sizeof(BIT_DIGIT)*8)
#define BALL   (~(BIT_DIGIT)0)
#define SLB(n) (((n)==NB)?~(BIT_DIGIT)0:(~(~(BIT_DIGIT)0<<(n))))

//#include "template.h"

#define ELEMENT_BIT_SIZE  "ELEMENT_BIT_SIZE"
#define ELEMENT_BYTE_SIZE "ELEMENT_BYTE_SIZE"
#define CONTIGUOUS_STRIDE "CONTIGUOUS_STRIDE"

#endif /* ifndef NARRAY_H */
