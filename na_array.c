/*
  na_array.c
  Numerical Array Extention for Ruby
    (C) Copyright 1999-2001 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#include <ruby.h>
#include "narray.h"

/* search max rank of an array */
static int
 na_search_rank(struct RArray *ary)
{
  int i, r, rmax=0;
  struct NARRAY *na;
  VALUE v;

  if (ary->len==0) return 0;    /* NIL if empty array */

  for (i=0; i<ary->len; i++) {
    /* Find rank number beneath this array */
    v = ary->ptr[i];
    if (TYPE(v) == T_ARRAY) {
      r = na_search_rank(RARRAY(v));
      if (r > rmax)
	rmax = r;
    }
    else if (IsNArray(v)) {
      GetNArray(v, na);
      r = na->rank;
      if (r > rmax)
	rmax = r;
    }
  }
  return rmax+1;
}


/* search each rank of an array */
static void
 na_search_shape(struct RArray *ary, int rank, int *shape)
{
  int i, j, r;
  struct NARRAY *na;
  VALUE v;

  if (ary->len > shape[rank])
    shape[rank] = ary->len;

  for (i=0; i < ary->len; i++) {
    v = ary->ptr[i];
    if (TYPE(v) == T_ARRAY)
      na_search_shape(RARRAY(v),rank-1,shape);
    else
    if (IsNArray(v)) {
      GetNArray(v,na);
      for ( j=0,r=rank-na->rank ; r < rank ; r++,j++ )
	if ( shape[r] < na->shape[j] )
	  shape[r] = na->shape[j];
    }
  }
}


/* get index from multiple-index  */
static int
 na_index_pos(struct NARRAY *ary, int *idxs)
{
  int i, idx, pos = 0;

  for ( i = ary->rank; (i--)>0; ) {
    idx = idxs[i];
    if (idx < 0 || ary->shape[i] <= idx)
      rb_raise(rb_eRuntimeError, "Subsctipt out of range");

    pos = pos * ary->shape[i] + idx;
  }
  return pos;
}


static void
 na_copy_nary_to_nary(VALUE obj, struct NARRAY *dst,
		      int thisrank, int *idx)
{
  struct NARRAY *src;
  struct slice *s;
  int  i, n;

  GetNArray(obj,src);
  n = thisrank - src->rank + 1;

  s = ALLOCA_N(struct slice, dst->rank+1);
  for (i=0; i < n; i++) {
    s[i].n    = 1;
    s[i].beg  = 0;
    s[i].step = 0;
    s[i].idx  = NULL;
  }
  for (   ; i <= thisrank; i++) {
    s[i].n    = src->shape[i-n];
    s[i].beg  = 0;
    s[i].step = 1;
    s[i].idx  = NULL;
  }
  for (   ; i < dst->rank; i++) {
    s[i].n    = 1;
    s[i].beg  = idx[i];
    s[i].step = 0;
    s[i].idx  = NULL;
  }
  na_aset_slice(dst,src,s);
}


/* copy Array to NArray */
static void
 na_copy_ary_to_nary(struct RArray *ary, struct NARRAY *na,
		     int thisrank, int *idx, void (*func)())
{
  int i, j, pos;
  VALUE v;

  if (thisrank==0) {
    i = 0;
    for (; i < ary->len; i++) {
      idx[thisrank] = i;
      v = ary->ptr[i];
      /* NIL if empty */
      if (v != Qnil && !(TYPE(v) == T_ARRAY && RARRAY(v)->len == 0)) {
	pos = na_index_pos(na,idx);
	(*func)( 1, NA_PTR(na,pos), 0, &v, 0 );  /* copy here */
      }
    }
  } else {
    i = 0;
    for (; i < ary->len; i++) {

      idx[thisrank] = i;
      v = ary->ptr[i];
      if (TYPE(v) == T_ARRAY)
	na_copy_ary_to_nary(RARRAY(v),na,thisrank-1,idx,func);
      else
      if (IsNArray(v))
	na_copy_nary_to_nary(v,na,thisrank-1,idx);
      else {
	for (j=thisrank-1; j>=0; j--) idx[j] = 0;
	pos = na_index_pos(na,idx);
	(*func)( 1, NA_PTR(na,pos), 0, ary->ptr+i, 0 );  /* copy here */
      }
    }
  }
}


int na_object_type(VALUE v)
{
  switch(TYPE(v)) {

  case T_TRUE:
  case T_FALSE:
    return NA_BYTE;

  case T_FIXNUM:
  case T_BIGNUM:
    return NA_LINT;

  case T_FLOAT:
    return NA_DFLOAT;

  default:
    if (IsNArray(v))
      return ((struct NARRAY *)(RDATA(v)->data))->type ;

    if (CLASS_OF(v) == cComplex)
      return NA_DCOMPLEX;
  }
  return NA_ROBJ;
}



static void
 na_search_type(struct RArray *ary, int *check)
{
  int i;
  VALUE v;

  for (i=0; i<ary->len; i++) {
    v = ary->ptr[i];
    switch(TYPE(v)) {
    case T_ARRAY:
      na_search_type(RARRAY(v), check);
      break;
    case T_NIL:
      break;
    default:
      check[ na_object_type(v) ] = 1;
    }
  }
}


static int
 na_ary_search_type(VALUE ary)
{
  int  i, type, *check;

  check = ALLOCA_N(int,NA_NTYPES);
  for (i=0; i<NA_NTYPES; i++)
    check[i]=0;

  na_search_type(RARRAY(ary), check);

  type = NA_BYTE;
  for (i=0; i<NA_NTYPES; i++) {
    if ( check[i] > 0 )
      type = na_upcast[type][i];
  }
  return type;
}


static VALUE
 na_ary_to_nary_w_type(VALUE ary, int type, VALUE klass)
{
  int  i, rank;
  int *shape, *idx;
  struct NARRAY *na;
  VALUE v;

  /* empty array */
  if (RARRAY(ary)->len < 1) {
    return na_make_empty( type, klass );
  }

  /* get rank */
  rank  = na_search_rank(RARRAY(ary));
  shape = ALLOCA_N(int,rank);

  /* get size */
  for (i=0; i<rank; i++) shape[i]=0;
  na_search_shape(RARRAY(ary),rank-1,shape);

  /* Create NArray */
  v  = na_make_object(type,rank,shape,klass);
  GetNArray(v,na);

  na_clear_data(na);
  idx = ALLOCA_N(int,rank);  
  for (i=0; i<rank; i++) idx[i]=0;

  na_copy_ary_to_nary( RARRAY(ary), na, rank-1, idx, 
		       SetFuncs[type][NA_ROBJ] );
  return v;
}

VALUE
 na_ary_to_nary(VALUE ary, VALUE klass)
{
  return na_ary_to_nary_w_type( ary, na_ary_search_type(ary), klass );
}



/* NArray === obj */

VALUE
 na_dup_w_type(VALUE v2, int type)
{
  VALUE  v1;
  struct NARRAY *a1, *a2;

  GetNArray(v2,a2);
  v1 = na_make_object(type, a2->rank, a2->shape, CLASS_OF(v2));
  GetNArray(v1,a1);
  na_copy_nary(a1,a2);
  return v1;
}


VALUE
 na_change_type(VALUE obj, int type)
{
  struct NARRAY *a2;

  GetNArray(obj,a2);

  if (a2->type == type)
    return obj;

  return na_dup_w_type(obj, type);
}


VALUE
 na_upcast_type(VALUE obj, int type)  /* na_upcast_narray */
{
  int newtype;
  struct NARRAY *a2;

  GetNArray(obj,a2);
  newtype = na_upcast[a2->type][type];

  if (newtype == a2->type)
    return obj;

  return na_dup_w_type(obj, newtype);
}


/* Object === obj */

VALUE
 na_cast_object(VALUE obj, int type) /* na_cast_certain */
{
  if (IsNArray(obj)) {
    return na_change_type(obj,type);
  }
  if (TYPE(obj) == T_ARRAY) {
    return na_ary_to_nary_w_type(obj,type,cNArray);
  }
  return na_make_scalar(obj,type);
}


VALUE
 na_cast_unless_narray(VALUE obj, int type)
{
  if (IsNArray(obj)) {
    return obj;
  }
  if (TYPE(obj) == T_ARRAY) {
    return na_ary_to_nary_w_type(obj,type,cNArray);
  }
  return na_make_scalar(obj,type);
}


VALUE
 na_cast_unless_array(VALUE obj, int type)
{
  if (IsNArray(obj)) {
    return obj;
  }
  if (TYPE(obj) == T_ARRAY) {
    return na_ary_to_nary(obj,cNArray);
  }
  return na_make_scalar(obj,type);
}


VALUE
 na_upcast_object(VALUE obj, int type)
{
  if (IsNArray(obj)) {
    return na_upcast_type(obj,type);
  }
  if (TYPE(obj) == T_ARRAY) {
    return na_ary_to_nary_w_type(obj,type,cNArray);
  }
  return na_make_scalar(obj,type);
}


VALUE
 na_to_narray(VALUE obj)
{
  if (IsNArray(obj)) {
    return obj;
  }
  if (TYPE(obj) == T_ARRAY) {
    return na_ary_to_nary(obj,cNArray);
  }
  return na_make_scalar(obj,na_object_type(obj));
}


/* convert NArray to Array */
static VALUE
 na_to_array0(struct NARRAY* na, int *idx, int thisrank, void (*func)())
{
  int i, pos;
  struct RArray *ary;

  /* Create New Array */
  ary = (struct RArray*)rb_ary_new2(na->shape[thisrank]);
  ary->len = na->shape[thisrank];

  if (thisrank == 0) {
    for (i = 0; i < na->shape[thisrank]; i++) {
      idx[thisrank] = i;
      pos = na_index_pos(na,idx);
      (*func)( 1, ary->ptr+i, 0, NA_PTR(na,pos), 0 );
    }
  }
  else {
    for (i = 0; i < na->shape[thisrank]; i++) {
      idx[thisrank] = i;
      ary->ptr[i] = na_to_array0(na,idx,thisrank-1,func);
    }
  }
  return (VALUE)ary;
}


/* method: to_a -- convert itself to Array */
VALUE
 na_to_array(VALUE obj)
{
  struct NARRAY *na;
  int *idx, i;

  GetNArray(obj,na);

  if (na->rank<1)
    return rb_ary_new();

  idx = ALLOCA_N(int,na->rank);
  for (i = 0; i<na->rank; i++) idx[i] = 0;
  return na_to_array0(na,idx,na->rank-1,SetFuncs[NA_ROBJ][na->type]);
}


static VALUE
 na_inspect_col( int n, char *p2, int p2step, void (*tostr)(),
		 VALUE sep, int rank )
{
  VALUE str=Qnil, tmp;
  int max_col = 77;
  int sep_len = RSTRING(sep)->len;

  if (n>0)
    (*tostr)(&str,p2);

  for (n--; n>0; n--) {
    p2 += p2step;
    (*tostr)(&tmp,p2);

    if (!NIL_P(sep)) rb_str_concat(str, sep);

    if (RSTRING(str)->len + RSTRING(tmp)->len + rank*4 + sep_len < max_col) {
      rb_str_concat(str, tmp);
    } else {
      rb_str_cat(str,"...",3);
      return str;
    }
  }
  return str;
}


/*
 *   Create inspect string ... under construction
 */

VALUE
 na_make_inspect(VALUE val)
{
  int   i, ii, rank, count_line=0, max_line=10;
  int  *si;
  struct NARRAY *ary;
  struct slice *s1;

  VALUE fs = rb_str_new(", ",2);

  GetNArray(val,ary);
  if (ary->total < 1) return rb_str_new(0, 0);

  /* Allocate Structure */
  rank = ary->rank;
  s1 = ALLOCA_N(struct slice, rank+1);
  si = ALLOCA_N(int,rank);
  na_set_slice_1obj(rank,s1,ary->shape);

  /* Iteration */
  na_init_slice(s1, rank, ary->shape, na_sizeof[ary->type]);
  i = rank;
  s1[i].p = ary->ptr;
  val = rb_str_new(0,0);
  for(;;) {
    /* set pointers */
    while (i > 0) {
      i--;
      rb_str_cat(val, "[ ", 2);
      s1[i].p = s1[i].pbeg + s1[i+1].p;
      si[i] = s1[i].n;
    }

    rb_str_concat(val, na_inspect_col( s1[0].n, s1[0].p, s1[0].pstep,
				       InspFuncs[ary->type], fs, rank ));

    /* rank up */
    do {
      rb_str_cat(val, " ]", 2);
      if ( ++i == rank ) return val;
    } while ( --si[i] == 0 );
    s1[i].p += s1[i].pstep;

    rb_str_concat(val, fs);
    rb_str_cat(val, "\n", 1);

    /* count check */
    if (++count_line>=max_line) {
      rb_str_cat(val, " ...", 4);
      return val;
    }
    /* indent */
    for (ii=i; ii<rank; ii++)
      rb_str_cat(val, "  ", 2);
  }
}
