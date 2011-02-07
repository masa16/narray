/*
  template_sort.c
  Numerical Array Extension for Ruby
    (C) Copyright 2007-2011 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/

/*
 *      qsort.c: standard quicksort algorithm
 *
 *      Modifications from vanilla NetBSD source:
 *        Add do ... while() macro fix
 *        Remove __inline, _DIAGASSERTs, __P
 *        Remove ill-considered "swap_cnt" switch to insertion sort,
 *        in favor of a simple check for presorted input.
 *
 *      CAUTION: if you change this file, see also qsort_arg.c
 *
 *      $PostgreSQL: pgsql/src/port/qsort.c,v 1.12 2006/10/19 20:56:22 tgl Exp $
 */

/*      $NetBSD: qsort.c,v 1.13 2003/08/07 16:43:42 agc Exp $   */

/*-
 * Copyright (c) 1992, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *        may be used to endorse or promote products derived from this software
 *        without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.      IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
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

//#define MED3(a,b,c)                                   \
//    (cmp(a,b) < 0 ?                                   \
//     (cmp(b,c) < 0 ? b : (cmp(a,c) < 0 ? c : a))      \
//   : (cmp(b,c) > 0 ? b : (cmp(a,c) < 0 ? a : c)))     \

#define MED3(a,b,c)                                     \
    (cmpgt(b,a) ?                                       \
      (cmpgt(c,b) ? b : (cmpgt(c,a) ? c : a))           \
    : (cmpgt(b,c) ? b : (cmpgt(c,a) ? a : c)))          \

/*
  while (pb <= pc && (r = cmp(pb, a)) <= 0) {
    if (r == 0) {
      swap(type, pa, pb);
      pa += es;
    }
    pb += es;
  }

  while (pb <= pc) {
    if (cmpgt(pb, a)) {
      break;
    } else if (cmpeq(pb, a)) {
      swap(type, pa, pb);
      pa += es;
    }
    pb += es;
  }
*/

/*
  idx = alloc_a(size_t,n);
  pos = data_pointer
  ptr = pos + idx[i];
  data = *ptr;

  val = *(type*)(ptr + idx[i]);

size_t *a;
val = *(type*)(ptr+*a);

#define swap(type,x,y) do{\
type tmp=*(type*)(ptr+*(a));\
*(type*)(ptr+*(a))=*(type*)(ptr+*(y));\
*(type*)(ptr+*(y))=tmp;}\


#define cmp(x,y)                                                      \
    ( isnan(*(double*)(ptr+*(x))) ? (isnan(*(double*)(ptr+*(y))) ? 0 : 1) :         \
      ( isnan(*(double*)(ptr+*(y))) ? -1 :                                    \
       ( (*(double*)(ptr+*(x))==*(double*)(ptr+*(y))) ? 0 :                         \
         (*(double*)(ptr+*(x)) > *(double*)(ptr+*(y))) ? 1 : -1) ))

 */


#define DEF_QSORT(tpname,type)                                         \
void                                                                    \
 tpname##_qsort(void *a, size_t n, size_t es)                           \
{                                                                       \
    char *pa, *pb, *pc, *pd, *pl, *pm, *pn;                             \
    int  d, r, presorted;                                               \
                                                                        \
 loop:                                                                  \
    if (n < 7) {                                                        \
        for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es)  \
            for (pl = pm; pl > (char *) a && cmpgt(pl - es, pl);        \
                 pl -= es)                                              \
                swap(type, pl, pl - es);                                \
        return;                                                         \
    }                                                                   \
    presorted = 1;                                                      \
    for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es) {    \
        if (cmpgt(pm - es, pm)) {                                       \
            presorted = 0;                                              \
            break;                                                      \
        }                                                               \
    }                                                                   \
    if (presorted)                                                      \
        return;                                                         \
    pm = (char *) a + (n / 2) * es;                                     \
    if (n > 7) {                                                        \
        pl = (char *) a;                                                \
        pn = (char *) a + (n - 1) * es;                                 \
        if (n > 40) {                                                   \
            d = (n / 8) * es;                                           \
            pl = MED3(pl, pl + d, pl + 2 * d);                          \
            pm = MED3(pm - d, pm, pm + d);                              \
            pn = MED3(pn - 2 * d, pn - d, pn);                          \
        }                                                               \
        pm = MED3(pl, pm, pn);                                          \
    }                                                                   \
    swap(type, a, pm);                                                  \
    pa = pb = (char *) a + es;                                          \
    pc = pd = (char *) a + (n - 1) * es;                                \
    for (;;) {                                                          \
        while (pb <= pc && (r = cmp(pb, a)) <= 0) {                     \
            if (r == 0) {                                               \
                swap(type, pa, pb);                                     \
                pa += es;                                               \
            }                                                           \
            pb += es;                                                   \
        }                                                               \
        while (pb <= pc && (r = cmp(pc, a)) >= 0) {                     \
            if (r == 0) {                                               \
                swap(type, pc, pd);                                     \
                pd -= es;                                               \
            }                                                           \
            pc -= es;                                                   \
        }                                                               \
        if (pb > pc)                                                    \
            break;                                                      \
        swap(type, pb, pc);                                             \
        pb += es;                                                       \
        pc -= es;                                                       \
    }                                                                   \
    pn = (char *) a + n * es;                                           \
    r = Min(pa - (char *) a, pb - pa);                                  \
    vecswap(type, a, pb - r, r);                                        \
    r = Min(pd - pc, pn - pd - es);                                     \
    vecswap(type, pb, pn - r, r);                                       \
    if ((r = pb - pa) > es)                                             \
        tpname##_qsort(a, r / es, es);                                  \
    if ((r = pd - pc) > es) {                                           \
        a = pn - r;                                                     \
        n = r / es;                                                     \
        goto loop;                                                      \
    }                                                                   \
}

// ---------------------------------------------------------------------

#define DEF_SORT(tpname,type)                                           \
static void                                                             \
iter_##tpname##_sort(na_loop_t *const lp)                               \
{                                                                       \
    size_t i, n;                                                        \
    char *ptr;                                                          \
    ssize_t step;                                                       \
    size_t *idx;                                                        \
    type *buf;                                                          \
                                                                        \
    INIT_COUNTER(lp, n);                                                \
    INIT_PTR(lp, 0, ptr, step, idx);                                    \
    if (idx) {                                                          \
        /*Data_Get_Struct(lp->info, type, buf);*/                       \
        buf = (type*)(lp->opt_ptr);                                     \
        for (i=0; i<n; i++) {                                           \
            buf[i] = *(type*)(ptr+idx[i]);                              \
        }                                                               \
        tpname##_qsort(buf, n, sizeof(type));                           \
        for (i=0; i<n; i++) {                                           \
            *(type*)(ptr+idx[i]) = buf[i];                              \
        }                                                               \
    } else {  /*printf("ptr=0x%x n=%d step=%d\n",ptr,n,step);*/         \
        tpname##_qsort(ptr, n, step);                                   \
    }                                                                   \
}                                                                       \
static VALUE                                                            \
 nary_##tpname##_sort(int argc, VALUE *argv, VALUE self)                \
{                                                                       \
    return na_sort_main(argc, argv, self, iter_##tpname##_sort);        \
}

// ---------------------------------------------------------------------

#define DEF_SORT_INDEX(tpname)                                          \
static VALUE                                                            \
 nary_##tpname##_sort_index(int argc, VALUE *argv, VALUE self)          \
{                                                                       \
    return na_sort_index_main(argc, argv, self, tpname##_index_qsort);  \
}

/*
#define DEF_SORT_INDEX(tpname,tpclass,type)                     \
static void                                                     \
 tpname##_sort_index_loop(na_loop_t *const lp)                  \
{                                                               \
    tpname##_index_qsort(lp->args[0].ptr + lp->iter[0].pos,     \
                         lp->n[0], lp->iter[0].step );          \
}                                                               \
static VALUE                                                    \
nary_##tpname##_sort_index(int argc, VALUE *argv, VALUE self)   \
{                                                               \
    volatile VALUE cont;                                        \
    VALUE ptr, view, copy, idxclass;                            \
    ndfunc_t *func;                                             \
    narray_t *na;                                               \
    void     *beg;                                              \
    ssize_t   stp;                                              \
                                                                \
    if (!na_check_ladder(self,0)) {                             \
        cont = na_copy(self);                                   \
    } else {                                                    \
        cont = self;                                            \
    }                                                           \
    GetNArray(cont,na);                                         \
    if (na->size > (~(u_int32_t)0)) {                           \
        idxclass = cInt64;                                      \
    } else {                                                    \
        idxclass = cInt32;                                      \
    }                                                           \
    ptr = nary_pointer_new(cont);                               \
    beg = na_data_pointer_for_read(cont) + na->offset;          \
    stp = na->stride[na->ndim-1];                               \

    RNARRAY_MARK(ptr) = RNARRAY_MARK(self);                     \
    view = na_flatten_by_mark(argc, argv, ptr);                 \

    if (!na_check_ladder(view,-1)) {                            \
        copy = na_copy(view);                                   \
    } else {                                                    \
        copy = view;                                            \
    }                                                           \
    func = ndfunc_alloc(tpname##_sort_index_loop, NO_LOOP,      \
                        1, 0, Qnil);                            \
    func->args[0].dim = 1;                                      \
    ndloop_do(func, 1, copy);                              \
    ndfunc_free(func);                                          \
    if (view!=copy) {                                           \
        na_store(view, copy);                                   \
    }                                                           \
    return nary_pointer_index(ptr, beg, stp, idxclass);         \
}
*/


#define DEF_MEDIAN(tpname,type)                                         \
static void                                                             \
 iter_##tpname##_median(na_loop_t *const lp)                            \
{                                                                       \
    size_t i, n;                                                        \
    char *p1, *p2;                                                      \
    ssize_t s1, s2;                                                     \
    size_t *idx1, *idx2;                                                \
    type *buf;                                                          \
                                                                        \
    INIT_COUNTER(lp, n);                                                \
    INIT_PTR(lp, 0, p1, s1, idx1);                                      \
    INIT_PTR(lp, 1, p2, s2, idx2);                                      \
    buf = (type*)(lp->opt_ptr);                                         \
    if (idx1) {                                                         \
        for (i=0; i<n; i++) {                                           \
            buf[i] = *(type*)(p1+idx1[i]);                              \
        }                                                               \
    } else {                                                            \
        for (i=0; i<n; i++) {                                           \
            buf[i] = *(type*)(p1+s1*i);                                 \
        }                                                               \
    }                                                                   \
    tpname##_qsort(buf, n, sizeof(type));                               \
    for (; n; n--) {                                                    \
        if (!isnan(buf[n-1])) break;                                    \
    }                                                                   \
    if (n==0) {                                                         \
        *(type*)p2 = buf[0];                                            \
    }                                                                   \
    else if (n%2==0) {                                                  \
        *(type*)p2 = (buf[n/2-1]+buf[n/2])/2;                           \
    }                                                                   \
    else {                                                              \
        *(type*)p2 = buf[(n-1)/2];                                      \
    }                                                                   \
}                                                                       \
static VALUE                                                            \
 nary_##tpname##_median(int argc, VALUE *argv, VALUE self)              \
{                                                                       \
    VALUE v;                                                            \
    v = na_median_main(argc, argv, self, iter_##tpname##_median);       \
    return nary_##tpname##_extract(v);                                  \
}



/*
#define DEF_MEDIAN(tpname,tpclass,type)                        \
static void                                                     \
 tpname##_median_loop(na_loop_t *const lp)                      \
{                                                               \
    size_t   n;                                                 \
    char    *p1, *p2;                                           \
    ssize_t  s1, s2;                                            \
    ssize_t *idx1, *idx2;                                       \
    type     x, y;                                              \
    INIT_COUNTER(lp, n);                                        \
    INIT_PTR(lp, 0, p1, s1, idx1);                              \
    INIT_PTR(lp, 1, p2, s2, idx2);                              \
    tpname##_qsort(p1, n, s1);                                  \
    if (n%2==0) {                                               \
        x = *(type*)(p1+s1*(n/2-1));                            \
        y = *(type*)(p1+s1*(n/2));                              \
        x = (x+y)/2;                                            \
        *(type*)p2 = x;                                         \
    } else {                                                    \
        x = *(type*)(p1+s1*((n-1)/2));                          \
        *(type*)p2 = x;                                         \
    }                                                           \
}                                                               \
static VALUE                                                    \
 nary_##tpname##_median(int argc, VALUE *argv, VALUE self)      \
{                                                               \
    VALUE     view, copy, v;                                    \
    ndfunc_t *func;                                             \
    narray_t *na;                                               \
                                                                \
    view = na_flatten_by_mark(argc, argv, self);                \
    GetNArray(view,na);                                         \
    if (na->ndim<1) return self;                                \
        copy = na_copy(view);                                   \
    func = ndfunc_alloc(tpname##_median_loop, NO_LOOP,          \
                        1, 1, Qnil, tpclass);                   \
    func->args[0].dim = 1;                                      \
    v = ndloop_do(func, 1, copy);                               \
    ndfunc_free(func);                                          \
    return nary_##tpname##_extract(v);                          \
}
*/
