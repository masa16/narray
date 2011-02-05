/*
  dfloat.c
  Numerical Array Extention for Ruby
    (C) Copyright 1999-2011 by Masahiro TANAKA

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
//#include "bytedata.h"

/*
#include "template_int.h"
*/
#include "template_sort.h"
#include "SFMT.h"

#include "template.h"
#include "template_data.h"
#include "template_cast.h"

EXTERN double round(double);
EXTERN double log2(double);
EXTERN double exp2(double);
EXTERN double exp10(double);

extern VALUE mNMathDFloat;

#define DFLOAT2VALUE(p)  rb_float_new(*(double*)(p))
#define VALUE2DFLOAT(p)  NUM2DBL(*(VALUE*)(p))


VALUE
nary_dfloat_extract( VALUE self )
{
    volatile VALUE v;
    char *ptr;
    narray_t *na;
    GetNArray(self,na);

    if (na->ndim==0) {
        ptr = na_get_pointer_for_read(self) + na_get_offset(self);
        v = rb_float_new(*(double*)ptr);
        na_release_lock(self);
        return v;
    }
    return self;
}

DEF_FILL_DATA_WITH_NUM( dfloat, double, cDFloat, y=NUM2DBL(x) )

CAST_NUM_TO_DATA( dfloat, double, cDFloat, y=NUM2DBL(x) )
CAST_DATA_TO_ARRAY( dfloat, double, cDFloat, y=rb_float_new(x) )
CAST_ARRAY_TO_DATA( dfloat, double, cDFloat, z=NUM2DBL(x), z=y, z=0 )
CAST_INT_TO_DATA( int32,  int32_t,  dfloat, double, cDFloat, y=x )
/*
CAST_INT_TO_DATA( uint32, u_int32_t, dfloat, double, cDFloat, y=x )
CAST_INT_TO_DATA( int64,  int64_t,  dfloat, double, cDFloat, y=x )
CAST_INT_TO_DATA( uint64, u_int64_t, dfloat, double, cDFloat, y=x )
*/


static VALUE
nary_dfloat_s_cast(VALUE type, VALUE obj)
{
    VALUE r;

    if (CLASS_OF(obj)==cDFloat) {
        return obj;
    }
    if (TYPE(obj)==T_FLOAT || FIXNUM_P(obj) || TYPE(obj)==T_BIGNUM)  {
        return nary_cast_numeric_to_dfloat(obj);
    }
    if (TYPE(obj)==T_ARRAY) {
        return nary_cast_array_to_dfloat(obj);
    }
    if (rb_obj_is_kind_of(obj,cInt32)) {
        return nary_cast_int32_to_dfloat(obj);
    }
    /*
    if (rb_obj_is_kind_of(obj,cUInt32)) {
        return nary_cast_uint32_to_dfloat(obj);
    }
    if (rb_obj_is_kind_of(obj,cUInt64)) {
        return nary_cast_uint64_to_dfloat(obj);
    }
    if (rb_obj_is_kind_of(obj,cInt64)) {
        return nary_cast_int64_to_dfloat(obj);
    }
    */

    if ( IsNArray(obj) ) {
        r = rb_funcall( obj, rb_intern("coerce_cast"), 1, cDFloat );
        if (RTEST(r)) {
            return r;
        }
    }

    rb_raise(nary_eCastError, "unknown conversion from %s to %s",
             rb_class2name(CLASS_OF(obj)),
             rb_class2name(type));
    return r;
}

static VALUE
nary_dfloat_coerce_cast( VALUE value, VALUE type )
{
    return Qnil;
}


// ---------------------------------------------------------------------

static void
dfloat_print_loop( char *ptr, size_t pos, VALUE opt )
{
    printf("%g", *(double*)( ptr + pos ));
}

//VALUE
//nary_dfloat_debug_print( VALUE ary )
//{
//    ndfunc_debug_print( ary, dfloat_print_loop, Qnil );
//    return Qnil;
//}

static VALUE
format_dfloat( VALUE fmt, void *x )
{
    // fix-me
    char s[32];
    int n;

    if (NIL_P(fmt)) {
        n = sprintf(s,"%g",*(double*)x);
        return rb_str_new(s,n);
    }
    return rb_funcall( fmt, '%', 1, rb_float_new(*(double*)x) );
}

DEF_INSPECT(dfloat)
DEF_FORMAT(dfloat)

// ---------------------------------------------------------------------

DEF_BINARY(dfloat,cDFloat,double,add,x+=y)
DEF_BINARY(dfloat,cDFloat,double,sub,x-=y)
DEF_BINARY(dfloat,cDFloat,double,mul,x*=y)
DEF_BINARY(dfloat,cDFloat,double,div,x/=y)
DEF_BINARY(dfloat,cDFloat,double,mod,x=fmod(x,y))
DEF_BINARY(dfloat,cDFloat,double,pow,x=pow(x,y))

POW_DATA_INT(double)
DEF_BINARY_DI(dfloat,cDFloat,double,int32,cInt32,int32_t,pow,{x=pow_double_int(x,y);})

static VALUE
nary_dfloat_pow_main(VALUE self, VALUE other)
{
    if (FIXNUM_P(other) || rb_obj_is_kind_of(other,cInt32)) {
        return nary_dfloat_pow_int32(self,other);
    } else {
        return nary_dfloat_pow(self,other);
    }
}
static VALUE
nary_dfloat_s_pow_main(VALUE mod, VALUE self, VALUE other)
{
    if (FIXNUM_P(other) || rb_obj_is_kind_of(other,cInt32)) {
        return nary_dfloat_s_pow_int32(cDFloat,self,other);
    } else {
        return nary_dfloat_s_pow(cDFloat,self,other);
    }
}

DEF_UNARY(dfloat,cDFloat,double,abs,    x=fabs(x))
DEF_UNARY(dfloat,cDFloat,double,minus,  x=-x)
DEF_UNARY(dfloat,cDFloat,double,inverse,x=1.0/x)
DEF_UNARY(dfloat,cDFloat,double,floor,  x=floor(x))
DEF_UNARY(dfloat,cDFloat,double,round,  x=round(x))
DEF_UNARY(dfloat,cDFloat,double,ceil,   x=ceil(x))

DEF_CMP_BINARY(dfloat,cDFloat,double,eq,x==y)
DEF_CMP_BINARY(dfloat,cDFloat,double,ne,x!=y)
DEF_CMP_BINARY(dfloat,cDFloat,double,gt,x>y)
DEF_CMP_BINARY(dfloat,cDFloat,double,ge,x>=y)
DEF_CMP_BINARY(dfloat,cDFloat,double,lt,x<y)
DEF_CMP_BINARY(dfloat,cDFloat,double,le,x<=y)

DEF_CMP_UNARY(dfloat,cDFloat,double,isnan,isnan(x))
DEF_CMP_UNARY(dfloat,cDFloat,double,isinf,isinf(x))
DEF_CMP_UNARY(dfloat,cDFloat,double,isfinite,isfinite(x))

DEF_ACCUM_UNARY(dfloat,cDFloat,double,sum,
                {func->args[1].init = INT2FIX(0);},
                (!isnan(x) && x!=0), {x+=y;} )
DEF_ACCUM_UNARY(dfloat,cDFloat,double,accum,
                {func->args[1].init = INT2FIX(0); NDF_SET(func,NDF_KEEP_DIM);},
                (!isnan(x) && x!=0), {x+=y;} )
DEF_ACCUM_UNARY(dfloat,cDFloat,double,min,
                {func->args[1].init = nary_dfloat_new_dim0( 0.0/0.0 );},
                (!isnan(x) && (isnan(y) || y>x)), {} )
DEF_ACCUM_UNARY(dfloat,cDFloat,double,max,
                {func->args[1].init = nary_dfloat_new_dim0( 0.0/0.0 );},
                (!isnan(x) && (isnan(y) || y<x)), {} )

DEF_SEQ_DATA( dfloat, cDFloat, double, y=x )

DEF_RAND_DATA( dfloat, cDFloat, double, rand, {y=to_res53(gen_rand64());} )
DEF_RAND_NORM_DATA( dfloat, cDFloat, double, {rand_norm(a);} )

#define cmp(a,b)                                                      \
    ( isnan(*(double*)(a)) ? (isnan(*(double*)(b)) ? 0 : 1) :         \
      (isnan(*(double*)(b)) ? -1 :                                    \
       ( (*(double*)(a)==*(double*)(b)) ? 0 :                         \
         (*(double*)(a) > *(double*)(b)) ? 1 : -1) ))
#define cmpgt(a,b)                                              \
    ( (isnan(*(double*)(a)) && !isnan(*(double*)(b))) ||        \
      (*(double*)(a) > *(double*)(b)) )

DEF_QSORT(dfloat,double)
DEF_SORT(dfloat,double)

#undef val
#undef cmp
#undef cmpgt

/*
#define val(ptr,idx) (*(double*)((ptr)+*(size_t*)(idx)))

#define cmp(p,a,b)                                  \
    ( isnan(val(p,a)) ? (isnan(val(p,b)) ? 0 : 1) : \
      ( isnan(val(p,b)) ? -1 :                       \
        ( (val(p,a) == val(p,b)) ? 0 :                 \
          (val(p,a) > val(p,b)) ? 1 : -1) ))

#define cmpgt(p,a,b)                           \
    ( (isnan(val(p,a)) && !isnan(val(p,b))) || \
      (val(p,a) > val(p,b)) )

#define cmpeq(p,a,b)                          \
     ( (isnan(val(p,a)) && isnan(val(p,b))) ||      \
      (val(p,a) == val(p,b)) )
*/

#define cmp(a,b)                                                        \
    ( isnan(**(double**)(a)) ? (isnan(**(double**)(b)) ? 0 : 1) :       \
      ( isnan(**(double**)(b)) ? -1 :                                   \
        ( (**(double**)(a)==**(double**)(b)) ? 0 :                      \
          (**(double**)(a) > **(double**)(b)) ? 1 : -1) ))
#define cmpgt(a,b)                                              \
    ( (isnan(**(double**)(a)) && !isnan(**(double**)(b))) ||    \
      (**(double**)(a) > **(double**)(b)) )

/*
#define type double

void
 dfloat_index_qsort(void *a, size_t n, size_t es)
{
    char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
    int  d, r, presorted;

 loop:
    if (n < 7) {
        for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es)
            for (pl = pm; pl > (char *) a && cmpgt(pl - es, pl);
                 pl -= es)
                swap(type, pl, pl - es);
        return;
    }
    presorted = 1;
    for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es) {
        if (cmpgt(pm - es, pm)) {
            presorted = 0;
            break;
        }
    }
    if (presorted)
        return;
    pm = (char *) a + (n / 2) * es;
    if (n > 7) {
        pl = (char *) a;
        pn = (char *) a + (n - 1) * es;
        if (n > 40) {
            d = (n / 8) * es;
            pl = MED3(pl, pl + d, pl + 2 * d);
            pm = MED3(pm - d, pm, pm + d);
            pn = MED3(pn - 2 * d, pn - d, pn);
        }
        pm = MED3(pl, pm, pn);
    }
    swap(type, a, pm);
    pa = pb = (char *) a + es;
    pc = pd = (char *) a + (n - 1) * es;
    for (;;) {
        while (pb <= pc && (r = cmp(pb, a)) <= 0) {
            if (r == 0) {
                swap(type, pa, pb);
                pa += es;
            }
            pb += es;
        }
        while (pb <= pc && (r = cmp(pc, a)) >= 0) {
            if (r == 0) {
                swap(type, pc, pd);
                pd -= es;
            }
            pc -= es;
        }
        if (pb > pc)
            break;
        swap(type, pb, pc);
        pb += es;
        pc -= es;
    }
    pn = (char *) a + n * es;
    r = Min(pa - (char *) a, pb - pa);
    vecswap(type, a, pb - r, r);
    r = Min(pd - pc, pn - pd - es);
    vecswap(type, pb, pn - r, r);
    if ((r = pb - pa) > es)
        dfloat_index_qsort(a, r / es, es);
    if ((r = pd - pc) > es) {
        a = pn - r;
        n = r / es;
        goto loop;
    }
}
#undef type
*/

DEF_QSORT(dfloat_index,double*)
DEF_SORT_INDEX(dfloat)

DEF_MEDIAN(dfloat,double)
// mean
// sort
// median
// Math
// histogram

// y = a[0] + a[1]*x + a[2]*x^2 + a[3]*x^3 + ... + a[n]*x^n
//   = a[0] + x*(a[1] + x*(a[2] + x*(a[3] + x*(... + x*a[n] ))))


static void
iter_dfloat_poly(na_loop_t *const lp)
{
    size_t  i;
    char   *p2;
    ssize_t s2;
    ssize_t *idx2;
    double  x, y, a;

    x = *(double*)(lp->args[0].ptr + lp->iter[0].pos);
    i = lp->narg - 2;
    y = *(double*)(lp->args[i].ptr + lp->iter[i].pos);
    //printf("a[%d]=%f\n",i-1,y);
    for (; --i;) {
        y *= x;
        a = *(double*)(lp->args[i].ptr + lp->iter[i].pos);
        //printf("a[%d]=%f\n",i-1,a);
        y += a;
    }
    i = lp->narg - 1;
    *(double*)(lp->args[i].ptr + lp->iter[i].pos) = y;
}


static VALUE
nary_dfloat_poly(VALUE self, VALUE args)
{
    int argc, i;
    ndfunc_t *func;
    VALUE *types;
    VALUE *argv;
    volatile VALUE v, a;

    argc = RARRAY_LEN(args);
    types = ALLOCA_N(VALUE,argc+2);
    for (i=0; i<argc+2; i++) {
        types[i] = cDFloat;
    }
    argv = ALLOCA_N(VALUE,argc+1);
    argv[0] = self;
    for (i=0; i<argc; i++) {
        argv[i+1] = RARRAY_PTR(args)[i];
    }
    a = rb_ary_new4(argc+1, argv);

    func = ndfunc_alloc2(iter_dfloat_poly, NO_LOOP, argc+1, 1, types);
    v = ndloop_do2(func, a);
    ndfunc_free(func);
    return nary_dfloat_extract(v);
}

/*
static void
dfloat_poly_loop(na_loop_t *const lp)
{
    size_t  i;
    char   *p2;
    ssize_t s2;
    ssize_t *idx2;
    double  x, y, a;

    x = *(double*)(lp->args[0].ptr + lp->iter[0].pos);
    INIT_PTR(lp, 1, p2, s2, idx2);
    i = lp->n[0]-1;
    if (idx2) {
	y = a = *(double*)(p2+idx2[i]);
	for (; i--; ) {
	    y *= x;
	    y += a = *(double*)(p2+idx2[i]);
	}
    } else {
	y = a = *(double*)(p2+s2*(i));
        printf("a[%d]=%f\n",i,a);
	for (; i--; ) {
	    y *= x;
	    y += a = *(double*)(p2+s2*i);
            printf("a[%d]=%f\n",i,a);
	}
    }
    *(double*)(lp->args[2].ptr + lp->iter[2].pos) = y;
}


static VALUE
nary_dfloat_poly(VALUE self, VALUE args)
{
    ndfunc_t *func;
    VALUE v;

    func = ndfunc_alloc(dfloat_poly_loop, NO_LOOP,
                        2, 1, cDFloat, cDFloat, cDFloat);
    func->args[1].dim = 1;
    v = ndloop_do(func, 2, self, args);
    ndfunc_free(func);
    return nary_dfloat_extract(v);
}
*/


void
Init_nary_dfloat()
{
    volatile VALUE hCast;
    //rb_require("stringio");

    cDFloat = rb_define_class_under(cNArray, "DFloat", cNArray);
    rb_define_const(cNArray, "Float64", cDFloat);

    rb_define_const(cDFloat, ELEMENT_BIT_SIZE,  INT2FIX(sizeof(double)*8));
    rb_define_const(cDFloat, ELEMENT_BYTE_SIZE, INT2FIX(sizeof(double)));
    rb_define_const(cDFloat, CONTIGUOUS_STRIDE, INT2FIX(sizeof(double)));

    rb_define_singleton_method(cNArray, "DFloat", nary_dfloat_s_cast, 1);
    rb_define_singleton_method(cDFloat, "cast", nary_dfloat_s_cast, 1);
    rb_define_singleton_method(cDFloat, "[]", nary_dfloat_s_cast, -2);
    rb_define_method(cDFloat, "coerce_cast", nary_dfloat_coerce_cast, 1);

    //rb_define_singleton_method(cDFloat, "zeros", nary_dfloat_s_zeros, -1);
    //rb_define_singleton_method(cDFloat, "ones", nary_dfloat_s_ones, -1);

    rb_define_method(cDFloat, "to_a", nary_cast_dfloat_to_array, 0);
    //rb_define_method(cDFloat, "split", nary_dfloat_split, 1);

    rb_define_method(cDFloat, "inspect", nary_dfloat_inspect, 0);
    rb_define_method(cDFloat, "format", nary_dfloat_format, -1);
    rb_define_method(cDFloat, "format_to_a", nary_dfloat_format_to_a, -1);

    rb_define_method(cDFloat, "fill", nary_dfloat_fill, 1);

    rb_define_singleton_method(cDFloat, "add", nary_dfloat_s_add, 2);
    rb_define_singleton_method(cDFloat, "sub", nary_dfloat_s_sub, 2);
    rb_define_singleton_method(cDFloat, "mul", nary_dfloat_s_mul, 2);
    rb_define_singleton_method(cDFloat, "div", nary_dfloat_s_div, 2);
    rb_define_singleton_method(cDFloat, "mod", nary_dfloat_s_mod, 2);
    rb_define_singleton_method(cDFloat, "pow", nary_dfloat_s_pow_main, 2);

    rb_define_method(cDFloat, "+",  nary_dfloat_add, 1);
    rb_define_method(cDFloat, "-",  nary_dfloat_sub, 1);
    rb_define_method(cDFloat, "*",  nary_dfloat_mul, 1);
    rb_define_method(cDFloat, "/",  nary_dfloat_div, 1);
    rb_define_method(cDFloat, "%",  nary_dfloat_mod, 1);
    rb_define_method(cDFloat, "**", nary_dfloat_pow_main, 1);

    rb_define_singleton_method(cDFloat, "eq", nary_dfloat_s_eq, 2);
    rb_define_singleton_method(cDFloat, "ne", nary_dfloat_s_ne, 2);
    rb_define_singleton_method(cDFloat, "gt", nary_dfloat_s_gt, 2);
    rb_define_singleton_method(cDFloat, "ge", nary_dfloat_s_ge, 2);
    rb_define_singleton_method(cDFloat, "lt", nary_dfloat_s_lt, 2);
    rb_define_singleton_method(cDFloat, "le", nary_dfloat_s_le, 2);

    rb_define_method(cDFloat, "eq", nary_dfloat_eq, 1);
    rb_define_method(cDFloat, "ne", nary_dfloat_ne, 1);
    rb_define_method(cDFloat, "gt", nary_dfloat_gt, 1);
    rb_define_method(cDFloat, "ge", nary_dfloat_ge, 1);
    rb_define_method(cDFloat, "lt", nary_dfloat_lt, 1);
    rb_define_method(cDFloat, "le", nary_dfloat_le, 1);
    rb_define_method(cDFloat, ">",  nary_dfloat_gt, 1);
    rb_define_method(cDFloat, ">=", nary_dfloat_ge, 1);
    rb_define_method(cDFloat, "<",  nary_dfloat_lt, 1);
    rb_define_method(cDFloat, "<=", nary_dfloat_le, 1);

    rb_define_method(cDFloat, "-@",    nary_dfloat_minus, 0);
    rb_define_method(cDFloat, "inverse", nary_dfloat_inverse, 0);
    rb_define_method(cDFloat, "abs",   nary_dfloat_abs, 0);
    rb_define_method(cDFloat, "floor", nary_dfloat_floor, 0);
    rb_define_method(cDFloat, "round", nary_dfloat_round, 0);
    rb_define_method(cDFloat, "ceil",  nary_dfloat_ceil, 0);

    rb_define_method(cDFloat, "sum", nary_dfloat_sum, -1);
    rb_define_method(cDFloat, "accum", nary_dfloat_accum, -1);
    rb_define_method(cDFloat, "min", nary_dfloat_min, -1);
    rb_define_method(cDFloat, "max", nary_dfloat_max, -1);

    rb_define_method(cDFloat, "seq", nary_dfloat_seq, -1);

    rb_define_method(cDFloat, "rand", nary_dfloat_rand, 0);
    rb_define_method(cDFloat, "rand_norm", nary_dfloat_rand_norm, 0);

    rb_define_method(cDFloat, "sort", nary_dfloat_sort, -1);
    //rb_define_method(cDFloat, "sort!", nary_dfloat_sort_bang, -1);
    rb_define_method(cDFloat, "sort_index", nary_dfloat_sort_index, -1);
    rb_define_method(cDFloat, "median", nary_dfloat_median, -1);

    rb_define_method(cDFloat, "poly", nary_dfloat_poly, -2);

    hCast = rb_hash_new();
    rb_define_const(cDFloat, "UPCAST", hCast);
    rb_hash_aset(hCast, rb_cArray,   cDFloat);
    rb_hash_aset(hCast, rb_cFixnum,  cDFloat);
    rb_hash_aset(hCast, rb_cBignum,  cDFloat);
    rb_hash_aset(hCast, rb_cInteger, cDFloat);
    rb_hash_aset(hCast, rb_cFloat,   cDFloat);
}
