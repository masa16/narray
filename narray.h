/*
  narray.h
  Numerical Array Extention for Ruby
    (C) Copyright 1999-2001 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#ifndef NARRAY_H
#define NARRAY_H

#include <math.h>

#include "narray_config.h"

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

/*
  Data types used in NArray :
  Please modify these types if your system has any different type.
*/
#ifndef HAVE_U_INT8_T
typedef unsigned char          u_int8_t; /* NA_BYTE */
#endif

#ifndef HAVE_INT16_T
# if SIZEOF_SHORT != 2
---->> Please typedef int16_t manually because sizeof(short) != 2. <<----
typedef 16_bit_integer_type    int16_t;
# else
typedef short                  int16_t;  /* NA_SINT */
# endif
#endif /* HAVE_INT16_T */

#ifndef HAVE_INT32_T
# if SIZEOF_LONG != 4
---->> Please typedef int32_t manually because sizeof(long) != 4. <<----
typedef 32_bit_integer_type    int32_t;
# else
typedef long                   int32_t;  /* NA_LINT */
# endif
#endif /* HAVE_INT32_T */

typedef struct { float r,i; }  scomplex;
typedef struct { double r,i; } dcomplex;

typedef int32_t na_index_t;

enum NArray_Types {
  NA_NONE,
  NA_BYTE,	/* 1 */
  NA_SINT,	/* 2 */
  NA_LINT,	/* 3 */
  NA_SFLOAT,	/* 4 */
  NA_DFLOAT,	/* 5 */
  NA_SCOMPLEX,	/* 6 */
  NA_DCOMPLEX,	/* 7 */
  NA_ROBJ,	/* 8 */
  NA_NTYPES	/* 9 */
};


/* struct for Numerical Array */
struct NARRAY {
  int    rank;	  /* # of dimension */
  int    total;	  /* # of total element */
  int    type;	  /* data type */
  int   *shape;
  char  *ptr;	  /* pointer to data */
  VALUE  ref;	  /* NArray object wrapping this structure */
};

struct slice {
  char *p;   	/* pointer to data		--- used in loop */
  int n;       	/* n of indices of this rank */
  int pstep;   	/* = step * stride * elmsz	--- set in na_init_slice */ 
  int pbeg;    	/* = beg * stride * elmsz	--- set in na_init_slice */
  int stride;  	/* = shape[0]*shape[1]*...*shape[r-1]
						--- set in na_init_slice */
  int step;
  int beg;
  na_index_t *idx;    	/* NULL if normal step */
};

#define NA_MAX_RANK 15

#define GetNArray(obj,var)  Data_Get_Struct(obj, struct NARRAY, var)
#define IsNArray(obj) (rb_obj_is_kind_of(obj,cNArray)==Qtrue)

#define NA_PTR(a,p)  ((a)->ptr+(p)*na_sizeof[(a)->type])
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

#define rb_complex_new(r, i) \
 rb_funcall(cComplex, na_id_new, 2, rb_float_new(r), rb_float_new(i))
#define NUM2REAL(v)  NUM2DBL( rb_funcall((v),na_id_real,0) )
#define NUM2IMAG(v)  NUM2DBL( rb_funcall((v),na_id_imag,0) )

#define NA_STRUCT(dta) ((struct NARRAY*)DATA_PTR(dta))

#define NA_ALLOC_SLICE(slc,nc,shp,np) \
{ slc = (struct slice*)xmalloc( sizeof(struct slice)*(nc) + \
				sizeof(int)*(np) );\
  shp = (int*)&( (slc)[nc] ); }


typedef void (*na_setfunc_t[NA_NTYPES][NA_NTYPES]) ();
typedef void (*na_func_t[NA_NTYPES]) ();
typedef void (*na_ufunc_t[NA_NTYPES]) ();
typedef void (*na_bifunc_t[NA_NTYPES]) ();
typedef void (*na_mathfunc_t[NA_NTYPES]) ();
typedef int  (*na_sortfunc_t[NA_NTYPES]) ();

/* function arrays */
extern na_setfunc_t SetFuncs;
extern na_ufunc_t  NegFuncs;
extern na_ufunc_t  RcpFuncs;
extern na_ufunc_t  AbsFuncs;
extern na_ufunc_t  RealFuncs;
extern na_ufunc_t  ImagFuncs;
extern na_ufunc_t  AnglFuncs;
extern na_ufunc_t  ImagMulFuncs;
extern na_ufunc_t  ConjFuncs;
extern na_ufunc_t  FloorFuncs;
extern na_ufunc_t  CeilFuncs;
extern na_ufunc_t  RoundFuncs;
extern na_ufunc_t  ToStrFuncs;
extern na_ufunc_t  InspFuncs;
extern na_ufunc_t  IndGenFuncs;
extern na_ufunc_t  AddUFuncs;
extern na_ufunc_t  SbtUFuncs;
extern na_ufunc_t  MulUFuncs;
extern na_ufunc_t  DivUFuncs;
extern na_bifunc_t AddBFuncs;
extern na_bifunc_t SbtBFuncs;
extern na_bifunc_t MulBFuncs;
extern na_bifunc_t DivBFuncs;
extern na_bifunc_t MulAddFuncs;
extern na_bifunc_t MulSbtFuncs;
extern na_bifunc_t ModBFuncs;
extern na_bifunc_t BAnFuncs;
extern na_bifunc_t BOrFuncs;
extern na_bifunc_t BXoFuncs;
extern na_ufunc_t  BRvFuncs;
extern na_bifunc_t ImgSetFuncs;
extern na_setfunc_t PowFuncs;
extern na_bifunc_t atan2Funcs;
extern na_bifunc_t CmpFuncs;
extern na_ufunc_t  AndFuncs;
extern na_ufunc_t  Or_Funcs;
extern na_ufunc_t  XorFuncs;
extern na_ufunc_t  NotFuncs;
extern na_ufunc_t  MinFuncs;
extern na_ufunc_t  MaxFuncs;
extern na_sortfunc_t SortFuncs;
extern na_sortfunc_t SortIdxFuncs;

/* variables */

extern VALUE cNArray, cNArrayScalar, cComplex;
extern VALUE rb_mNMath;
extern ID na_id_beg, na_id_end, na_id_exclude_end;
extern ID na_id_minus, na_id_abs, na_id_power;
extern ID na_id_compare, na_id_and, na_id_or;
extern ID na_id_class_dim;
extern ID na_id_add, na_id_sbt, na_id_mul, na_id_div, na_id_mod;
extern ID na_id_real, na_id_imag;
extern ID na_id_coerce_rev;
extern ID na_id_new;

extern int na_sizeof[NA_NTYPES+1];
extern int na_upcast[NA_NTYPES][NA_NTYPES];
extern int na_no_cast[NA_NTYPES];
extern int na_cast_real[NA_NTYPES];
extern int na_cast_comp[NA_NTYPES];
extern int na_cast_round[NA_NTYPES];
extern int na_cast_byte[NA_NTYPES];

extern char *na_typestring[];

/* Function Prototypes */

/* narray.c */
void  na_touch_object(VALUE val, ...);
VALUE na_make_object(int type, int rank, int *shape, VALUE klass);
VALUE na_make_scalar(VALUE obj, int type);
VALUE na_make_empty(int type, VALUE klass);
void  na_clear_data(struct NARRAY *ary);
VALUE na_clone(VALUE self);
VALUE na_fill(VALUE self, VALUE obj);
void  na_copy_nary(struct NARRAY *dst, struct NARRAY *src);

VALUE na_newdim_ref(int argc, VALUE *argv, VALUE self);

/* na_index.c */
VALUE na_aset(int argc, VALUE *argv, VALUE self);
VALUE na_aref(int argc, VALUE *argv, VALUE self);
VALUE na_slice(int argc, VALUE *argv, VALUE self);
void  na_aset_slice(struct NARRAY *dst, struct NARRAY *src, struct slice *s1);
int   na_shrink_class(int class_dim, int *shrink);
VALUE na_shrink_rank(VALUE obj, int class_dim, int *shrink);

/* na_array.c */
VALUE na_to_array(VALUE obj);
VALUE na_make_inspect(VALUE self);
VALUE na_ary_to_nary(VALUE ary, VALUE klass);
int   na_object_type(VALUE v);

VALUE na_cast_object(VALUE obj, int type);
VALUE na_cast_unless_narray(VALUE obj, int type);
VALUE na_cast_unless_array(VALUE obj, int type);
VALUE na_upcast_object(VALUE obj, int type);
VALUE na_cast_unless_array(VALUE obj, int type);
VALUE na_dup_w_type(VALUE obj, int type);
VALUE na_change_type(VALUE obj, int type);
VALUE na_upcast_type(VALUE obj, int type);
VALUE na_to_narray(VALUE obj);

/* na_func.c */
int  na_max3(int a, int b, int c);
void na_shape_max3(int ndim, int *max_shp, int *shp1, int *shp2, int *shp3);
void na_shape_copy( int ndim, int *shape, struct NARRAY *a );

void na_init_slice(struct slice *s, int rank, int *shape, int elmsz);
void na_set_slice_1obj(int ndim, struct slice *slc, int *shape);
int  na_set_slice_3obj( int ndim,
			struct slice *s1, struct slice *s2, struct slice *s3,
			int *shp1, int *shp2, int *shp3, int *shape );
void na_loop_general(struct NARRAY *a1, struct NARRAY *a2,
		     struct slice *s1, struct slice *s2, void (*func)());
void na_loop_index_ref(struct NARRAY *a1, struct NARRAY *a2,
		       struct slice *s1, struct slice *s2, void (*func)());

#ifdef DEBUG
#undef xfree
#define xfree(ptr) na_xfree(ptr)
void na_xfree(void *ptr);
#endif

#endif /* ifndef NARRAY_H */
