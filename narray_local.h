/*
  narray_local.h
  Numerical Array Extention for Ruby
    (C) Copyright 1999-2002 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/

typedef int32_t na_index_t;

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
extern na_bifunc_t RefMaskFuncs;
extern na_bifunc_t SetMaskFuncs;

/* variables */

extern VALUE rb_mNMath;
extern ID na_id_beg, na_id_end, na_id_exclude_end;
extern ID na_id_minus, na_id_abs, na_id_power;
extern ID na_id_compare, na_id_and, na_id_or;
extern ID na_id_class_dim;
extern ID na_id_add, na_id_sbt, na_id_mul, na_id_div, na_id_mod;
extern ID na_id_real, na_id_imag;
extern ID na_id_coerce_rev;
extern ID na_id_new;

extern const int na_upcast[NA_NTYPES][NA_NTYPES];
extern const int na_no_cast[NA_NTYPES];
extern const int na_cast_real[NA_NTYPES];
extern const int na_cast_comp[NA_NTYPES];
extern const int na_cast_round[NA_NTYPES];
extern const int na_cast_byte[NA_NTYPES];

extern const char *na_typestring[];

/* narray.c */
VALUE na_newdim_ref(int argc, VALUE *argv, VALUE self);

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

/* na_index.c */
void  na_aset_slice(struct NARRAY *dst, struct NARRAY *src, struct slice *s1);
int   na_shrink_class(int class_dim, int *shrink);
VALUE na_shrink_rank(VALUE obj, int class_dim, int *shrink);

#define rb_complex_new(r,i) \
  rb_funcall(cComplex, na_id_new, 2, rb_float_new(r), rb_float_new(i))
