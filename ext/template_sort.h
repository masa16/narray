/*
  template_sort.c
  Numerical Array Extention for Ruby
    (C) Copyright 2007 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/

/*
 *	qsort.c: standard quicksort algorithm
 *
 *	Modifications from vanilla NetBSD source:
 *	  Add do ... while() macro fix
 *	  Remove __inline, _DIAGASSERTs, __P
 *	  Remove ill-considered "swap_cnt" switch to insertion sort,
 *	  in favor of a simple check for presorted input.
 *
 *	CAUTION: if you change this file, see also qsort_arg.c
 *
 *	$PostgreSQL: pgsql/src/port/qsort.c,v 1.12 2006/10/19 20:56:22 tgl Exp $
 */

/*	$NetBSD: qsort.c,v 1.13 2003/08/07 16:43:42 agc Exp $	*/

/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *	  notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *	  notice, this list of conditions and the following disclaimer in the
 *	  documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *	  may be used to endorse or promote products derived from this software
 *	  without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.	IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define Min(x, y)               ((x) < (y) ? (x) : (y))

#define swap(type,a,b) \
    do {type tmp=*(type*)(a); *(type*)(a)=*(type*)(b); *(type*)(b)=tmp;} while(0)

#define vecswap(type, a, b, n) if ((n)>0) swap(type,(a),(b))

//#define MED3(a,b,c)					\
//    ( cmp(a,b) < 0 ?					\
//     (cmp(b,c) < 0 ? b : (cmp(a,c) < 0 ? c : a))	\
//   : (cmp(b,c) > 0 ? b : (cmp(a,c) < 0 ? a : c)) )	\

#define MED3(a,b,c)					\
    ( cmpgt(b,a) ?					\
      (cmpgt(c,b) ? b : (cmpgt(c,a) ? c : a))		\
    : (cmpgt(b,c) ? b : (cmpgt(c,a) ? a : c)) )		\


#define QSORT_FUNC(tpname,type)						\
void									\
 tpname##_qsort(void *a, size_t n, size_t es)				\
{									\
    char *pa, *pb, *pc, *pd, *pl, *pm, *pn;				\
    int	 d, r, presorted;						\
									\
 loop:									\
    if (n < 7) {							\
	for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es)	\
	    for (pl = pm; pl > (char *) a && cmpgt(pl - es, pl);	\
		 pl -= es)						\
		swap(type, pl, pl - es);				\
	return;								\
    }									\
    presorted = 1;							\
    for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es) {	\
	if (cmpgt(pm - es, pm)) {					\
	    presorted = 0;						\
	    break;							\
	}								\
    }									\
    if (presorted)							\
	return;								\
    pm = (char *) a + (n / 2) * es;					\
    if (n > 7) {							\
	pl = (char *) a;						\
	pn = (char *) a + (n - 1) * es;					\
	if (n > 40) {							\
	    d = (n / 8) * es;						\
	    pl = MED3(pl, pl + d, pl + 2 * d);				\
	    pm = MED3(pm - d, pm, pm + d);				\
	    pn = MED3(pn - 2 * d, pn - d, pn);				\
	}								\
	pm = MED3(pl, pm, pn);						\
    }									\
    swap(type, a, pm);							\
    pa = pb = (char *) a + es;						\
    pc = pd = (char *) a + (n - 1) * es;				\
    for (;;) {								\
	while (pb <= pc && (r = cmp(pb, a)) <= 0) {			\
	    if (r == 0) {						\
		swap(type, pa, pb);					\
		pa += es;						\
	    }								\
	    pb += es;							\
	}								\
	while (pb <= pc && (r = cmp(pc, a)) >= 0) {			\
	    if (r == 0) {						\
		swap(type, pc, pd);					\
		pd -= es;						\
	    }								\
	    pc -= es;							\
	}								\
	if (pb > pc)							\
	    break;							\
	swap(type, pb, pc);						\
	pb += es;							\
	pc -= es;							\
    }									\
    pn = (char *) a + n * es;						\
    r = Min(pa - (char *) a, pb - pa);					\
    vecswap(type, a, pb - r, r);					\
    r = Min(pd - pc, pn - pd - es);					\
    vecswap(type, pb, pn - r, r);					\
    if ((r = pb - pa) > es)						\
	tpname##_qsort(a, r / es, es);					\
    if ((r = pd - pc) > es) {						\
	a = pn - r;							\
	n = r / es;							\
	goto loop;							\
    }									\
}


#define SORT_FUNC(tpname,tpclass,type)				\
static void							\
 tpname##_sort_loop( na_iterator_t *const itr, VALUE opt )	\
{								\
    tpname##_qsort( itr[0].ptr+itr[0].pos, itr[0].n, itr[0].step );\
}								\
static VALUE							\
 num_##tpname##_sort_bang( int argc, VALUE *argv, VALUE self )	\
{								\
    VALUE     view, copy;					\
    ndfunc_t *func;						\
    int       ndim;						\
    narray_t *na;						\
								\
    view = na_flatten_by_mark( argc, argv, self );		\
    GetNArray(view,na);						\
    ndim = na->ndim;						\
    if (ndim<1) return self;					\
    if (CLASS_OF(self)!=tpclass) {				\
	copy = num_cast_##tpname##_to_##tpname(tpclass,view);	\
    } else if (!na_check_ladder(view,-1)) {			\
	copy = na_copy(view);					\
    } else {							\
	copy = view;						\
    }								\
    func = ndfunc_alloc( tpname##_sort_loop, NO_LOOP,		\
			 1, 0, Qnil );				\
    func->args[0].dim = 1;					\
    ndfunc_execute( func, 1, copy );				\
    ndfunc_free(func);						\
    if (view!=copy) {						\
	na_store(view, copy);					\
    }								\
    return self;						\
}								\
static VALUE							\
 num_##tpname##_sort( int argc, VALUE *argv, VALUE self )	\
{								\
    VALUE copy = na_copy(self);					\
    RNARRAY_MARK(copy) = RNARRAY_MARK(self);			\
    return num_##tpname##_sort_bang( argc, argv, copy );	\
}



#define SORT_INDEX_FUNC(tpname,tpclass,type)			\
static void							\
tpname##_sort_index_loop( na_iterator_t *const itr, VALUE opt )	\
{								\
    tpname##_index_qsort( itr[0].ptr+itr[0].pos,		\
			  itr[0].n, itr[0].step );		\
}								\
static VALUE							\
num_##tpname##_sort_index( int argc, VALUE *argv, VALUE self )	\
{								\
    volatile VALUE cont;					\
    VALUE ptr, view, copy, idxclass;				\
    ndfunc_t *func;						\
    narray_t *na;						\
    void     *beg;						\
    ssize_t   stp;						\
    								\
    if (CLASS_OF(self)!=tpclass) {				\
	cont = num_cast_##tpname##_to_##tpname(tpclass,self);	\
    } else if (!na_check_ladder(self,0)) {			\
	cont = na_copy(self);					\
    } else {							\
	cont = self;						\
    }								\
    GetNArray(cont,na);						\
    if (na->size > (~(u_int32_t)0) ) {				\
	idxclass = cInt64;					\
    } else {							\
	idxclass = cInt32;					\
    }								\
    ptr = num_pointer_new(cont);				\
    beg = na_data_pointer_for_read(cont) + na->offset;		\
    stp = na->stride[na->ndim-1];				\
    RNARRAY_MARK(ptr) = RNARRAY_MARK(self);			\
    view = na_flatten_by_mark( argc, argv, ptr );		\
    if (!na_check_ladder(view,-1)) {				\
	copy = na_copy(view);					\
    } else {							\
	copy = view;						\
    }								\
    func = ndfunc_alloc( tpname##_sort_index_loop, NO_LOOP,	\
			 1, 0, Qnil );				\
    func->args[0].dim = 1;					\
    ndfunc_execute( func, 1, copy );				\
    ndfunc_free(func);						\
    if (view!=copy) {						\
	na_store(view, copy);					\
    }								\
    return num_pointer_index( ptr, beg, stp, idxclass );	\
}



#define MEDIAN_FUNC(tpname,tpclass,type)			\
static void							\
 tpname##_median_loop( na_iterator_t *const itr, VALUE opt )	\
{								\
    size_t   n;							\
    char    *p1, *p2;						\
    ssize_t  s1, s2;						\
    ssize_t *idx1, *idx2;					\
    type     x, y;						\
    INIT_COUNTER( itr, n );					\
    INIT_PTR( itr, 0, p1, s1, idx1 );				\
    INIT_PTR( itr, 1, p2, s2, idx2 );				\
    tpname##_qsort( p1, n, s1 );				\
    if (n%2==0) {						\
	x = *(type*)(p1+s1*(n/2-1));				\
	y = *(type*)(p1+s1*(n/2));				\
	x = (x+y)/2;						\
	*(type*)p2 = x;						\
    } else {							\
	x = *(type*)(p1+s1*((n-1)/2));				\
	*(type*)p2 = x;						\
    }								\
}								\
static VALUE							\
 num_##tpname##_median( int argc, VALUE *argv, VALUE self )	\
{								\
    VALUE     view, copy, v;					\
    ndfunc_t *func;						\
    narray_t *na;						\
								\
    view = na_flatten_by_mark( argc, argv, self );		\
    GetNArray(view,na);						\
    if (na->ndim<1) return self;				\
    if (CLASS_OF(self)!=tpclass) {				\
	copy = num_cast_##tpname##_to_##tpname(tpclass,view);	\
    } else {							\
	copy = na_copy(view);					\
    }								\
    func = ndfunc_alloc( tpname##_median_loop, NO_LOOP,		\
			 1, 1, Qnil, tpclass );			\
    func->args[0].dim = 1;					\
    v = ndfunc_execute( func, 1, copy );			\
    ndfunc_free(func);						\
    return num_##tpname##_extract(v);				\
}
