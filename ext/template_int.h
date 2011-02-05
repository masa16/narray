/*
  template_int.c
  Numerical Array Extention for Ruby
    (C) Copyright 1999-2011 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/


#define DEBUG_PRINT_INT(intname,esz,type,fmt)   \
static void                                     \
 iter_##intname##_print(char *ptr, size_t pos, VALUE opt)      \
{                                               \
    type x;                                     \
    LOAD_INT(ptr+pos, esz, x);          \
    printf(fmt, x);                             \
}                                               \
static VALUE                                    \
 nary_##intname##_debug_print(VALUE ary)        \
{                                               \
    ndfunc_debug_print(ary, iter_##intname##_print, Qnil);      \
    return Qnil;                                \
}


#define DEF_BINARY_INT(tpname, tpclass, type, opname, operation)        \
static void                                                             \
 iter_##tpname##_##opname(na_loop_t *const lp)                         \
{                                                                       \
    size_t  i;                                                          \
    char   *p1, *p2, *p3;                                               \
    ssize_t s1, s2, s3;                                                 \
    ssize_t *idx1, *idx2, *idx3;                                        \
    int     e1, e2, e3;                                                 \
    type    x, y;                                                       \
    INIT_COUNTER(lp, i);                                               \
    INIT_PTR_ELM(lp, 0, p1, s1, idx1, e1);                             \
    INIT_PTR_ELM(lp, 1, p2, s2, idx2, e2);                             \
    INIT_PTR_ELM(lp, 2, p3, s3, idx3, e3);                             \
    if (idx1||idx2||idx3||                                              \
        e1!=sizeof(type)||e2!=sizeof(type)||e3!=sizeof(type)) {         \
        for (; i--;) {                                                  \
            LOAD_INT_STEP(p1, s1, idx1, e1, type, x);                   \
            LOAD_INT_STEP(p2, s2, idx2, e2, type, y);                   \
            operation;                                                  \
            STORE_INT_STEP(p3, s3, idx3, e3, type, x);                  \
        }                                                               \
    } else {                                                            \
        for (; i--;) {                                                  \
            x = *(type*)p1;                                             \
            p1+=s1;                                                     \
            y = *(type*)p2;                                             \
            p2+=s2;                                                     \
            operation;                                                  \
            *(type*)p3 = x;                                             \
            p3+=s3;                                                     \
        }                                                               \
    }                                                                   \
}                                                                       \
static VALUE                                                            \
 nary_##tpname##_s_##opname(VALUE mod, VALUE a1, VALUE a2)              \
{                                                                       \
    VALUE v, c1, c2;                                                    \
    ndfunc_t *func;                                                     \
    if (rb_obj_is_kind_of(a1,mod)) {                                    \
        c1 = CLASS_OF(a1);                                              \
    } else {                                                            \
        c1 = mod;                                                       \
    }                                                                   \
    if (rb_obj_is_kind_of(a2,mod)) {                                    \
        c2 = CLASS_OF(a2);                                              \
    } else {                                                            \
        c2 = mod;                                                       \
    }                                                                   \
    func = ndfunc_alloc(iter_##tpname##_##opname, FULL_LOOP,            \
                        2, 1, c1, c2, mod);                             \
    v = ndloop_do(func, 2, a1, a2);                                \
    ndfunc_free(func);                                                  \
    return v;                                                           \
}                                                                       \
static VALUE                                                            \
 nary_##tpname##_##opname(VALUE a1, VALUE a2)                           \
{                                                                       \
    VALUE klass;                                                        \
    klass = na_upcast(CLASS_OF(a1),CLASS_OF(a2));                       \
    if (TYPE(klass)==T_CLASS) {                                         \
        if (RTEST(rb_class_inherited_p(klass,tpclass))) {               \
            return nary_##tpname##_s_##opname(klass,a1,a2);             \
        }                                                               \
    }                                                                   \
    return rb_funcall(klass,id_##opname,2,a1,a2);                       \
}                                                                       \


#define DEF_UNARY_INT(tpname, tpclass, type, opname, operation) \
static void                                                             \
 tpname##_##opname##_loop(na_loop_t *const lp)                         \
{                                                                       \
    size_t  i;                                                          \
    char   *p1, *p2;                                                    \
    ssize_t s1, s2;                                                     \
    ssize_t *idx1, *idx2;                                               \
    int     e1, e2;                                                     \
    type    x;                                                          \
    INIT_COUNTER(lp, i);                                               \
    INIT_PTR_ELM(lp, 0, p1, s1, idx1, e1);                             \
    INIT_PTR_ELM(lp, 1, p2, s2, idx2, e2);                             \
    if (idx1||idx2||                                                    \
        e1!=sizeof(type)||e2!=sizeof(type)) {                           \
        for (; i--;) {                                                  \
            LOAD_INT_STEP(p1, s1, idx1, e1, type, x);                   \
            operation;                                                  \
            STORE_INT_STEP(p2, s2, idx2, e2, type, x);          \
        }                                                               \
    } else {                                                            \
        for (; i--;) {                                                  \
            x = *(type*)p1;                                             \
            p1+=s1;                                                     \
            operation;                                                  \
            *(type*)p2 = x;                                             \
            p2+=s2;                                                     \
        }                                                               \
    }                                                                   \
}                                                                       \
static VALUE                                                            \
nary_##tpname##_##opname(VALUE a1)                                      \
{                                                                       \
    VALUE v, klass;                                                     \
    ndfunc_t *func;                                                     \
    klass = CLASS_OF(a1);                                               \
    func = ndfunc_alloc(tpname##_##opname##_loop, FULL_LOOP,            \
                            1, 1, klass, klass);                        \
    v = ndloop_do(func, 1, a1);                                    \
    ndfunc_free(func);                                          \
    return v;                                                           \
}


#define DEF_CMP_BINARY_INT(tpname, tpclass, type, opname, operation)    \
static void                                                             \
 tpname##_##opname##_loop(na_loop_t *const lp)                         \
{                                                                       \
    size_t  i;                                                          \
    char   *p1, *p2;                                                    \
    BIT_DIGIT *a3;                                                      \
    size_t  p3;                                                         \
    ssize_t s1, s2, s3;                                                 \
    ssize_t *idx1, *idx2, *idx3;                                        \
    int     e1, e2;                                                     \
    type    x, y;                                                       \
    BIT_DIGIT b;                                                        \
    INIT_COUNTER(lp, i);                                               \
    INIT_PTR_ELM(lp, 0, p1, s1, idx1, e1);                             \
    INIT_PTR_ELM(lp, 1, p2, s2, idx2, e2);                             \
    INIT_PTR_BIT(lp, 2, a3, p3, s3, idx3);                             \
    if (idx1||idx2||idx3||                                              \
        e1!=sizeof(type)||e2!=sizeof(type)) {                           \
        for (; i--;) {                                                  \
            LOAD_INT_STEP(p1, s1, idx1, e1, type, x);                   \
            LOAD_INT_STEP(p2, s2, idx2, e2, type, y);                   \
            b = (operation) ? 1:0;                                      \
            STORE_BIT_STEP(a3, p3, s3, idx3, b);                        \
        }                                                               \
    } else {                                                            \
        for (; i--;) {                                                  \
            x = *(type*)p1;                                             \
            p1+=s1;                                                     \
            y = *(type*)p2;                                             \
            p2+=s2;                                                     \
            b = (operation) ? 1:0;                                      \
            STORE_BIT(a3,p3,b)                                          \
            p3+=s3;                                                     \
        }                                                               \
    }                                                                   \
}                                                                       \
static VALUE                                                            \
nary_##tpname##_s_##opname(VALUE mod, VALUE a1, VALUE a2)               \
{                                                                       \
    VALUE v;                                                            \
    ndfunc_t *func;                                                     \
    if (!rb_obj_is_kind_of(a1,tpclass))                                 \
        rb_raise(rb_eTypeError,"wrong class: %s, expected: %s",         \
                 rb_obj_classname(a1),rb_class2name(tpclass));          \
    if (!rb_obj_is_kind_of(a2,tpclass))                                 \
        rb_raise(rb_eTypeError,"wrong class: %s, expected: %s",         \
                 rb_obj_classname(a2),rb_class2name(tpclass));          \
    func = ndfunc_alloc(tpname##_##opname##_loop, FULL_LOOP,            \
                            2, 1, CLASS_OF(a1), CLASS_OF(a2), cBit);    \
    v = ndloop_do(func, 2, a1, a2);                                \
    ndfunc_free(func);                                                  \
    return v;                                                           \
}                                                                       \
static VALUE                                                            \
 nary_##tpname##_##opname(VALUE a1, VALUE a2)                           \
{                                                                       \
    VALUE klass;                                                        \
    klass = na_upcast(CLASS_OF(a1),CLASS_OF(a2));                       \
    if (klass==tpclass) {                                               \
        return nary_##tpname##_s_##opname(tpclass,a1,a2);               \
    }                                                                   \
    return rb_funcall(klass,id_##opname,2,a1,a2);                       \
}


#define DEF_ACCUM_UNARY_INT(tpname, tpclass, type, opname, init, condition, operation) \
static void                                                             \
 tpname##_##opname##_loop(na_loop_t *const lp)                         \
{                                                                       \
    size_t  i;                                                          \
    char   *p1, *p2;                                                    \
    type   *q2;                                                         \
    ssize_t s1, s2;                                                     \
    ssize_t *idx1, *idx2;                                               \
    int     e1, e2;                                                     \
    type    x, y;                                                       \
                                                                        \
    INIT_COUNTER(lp, i);                                               \
    INIT_PTR_ELM(lp, 0, p1, s1, idx1, e1);                             \
    INIT_PTR_ELM(lp, 1, p2, s2, idx2, e2);                             \
    if (idx1||idx2||                                                    \
        e1!=sizeof(type)||e2!=sizeof(type)) {                           \
        for (; i--;) {                                                  \
            LOAD_INT_STEP(p1, s1, idx1, e1, type, x);                   \
            LOAD_INT_TEMP(q2, p2, s2, idx2, e2, type, y);               \
            if (condition) {                                            \
                operation;                                              \
                STORE_INT(q2, e2, x);                                   \
            }                                                           \
        }                                                               \
    } else {                                                            \
        for (; i--;) {                                                  \
            x = *(type*)p1;                                             \
            p1+=s1;                                                     \
            y = *(type*)p2;                                             \
            if (condition) {                                            \
                {operation;}                                            \
                *(type*)p2 = x;                                         \
            }                                                           \
            p2+=s2;                                                     \
        }                                                               \
    }                                                                   \
}                                                                       \
static VALUE                                                            \
 nary_##tpname##_##opname(int argc, VALUE *argv, VALUE self)            \
{                                                                       \
    VALUE v, mark, klass;                                               \
    ndfunc_t *func;                                                     \
    mark = na_mark_dimension(argc, argv, self);                         \
    klass = CLASS_OF(self);                                             \
    func = ndfunc_alloc(tpname##_##opname##_loop, FULL_LOOP,            \
                        2, 1, klass, sym_mark, klass);                  \
    {init;};                                                            \
    v = ndloop_do(func, 2, self, mark);                                 \
    v = nary_##tpname##_extract(v);                                     \
    return v;                                                           \
}


#define DEF_INT_SEQ(intname, inttype)                           \
static void                                                     \
 intname##_seq(na_loop_t *const lp)                             \
{                                                               \
    size_t i;                                                   \
    char   *p1;                                                 \
    ssize_t  s1;                                                \
    ssize_t *idx1;                                              \
    size_t   e1;                                                \
    inttype  x;                                                 \
    double  beg, step;                                          \
    size_t  c;                                                  \
    seq_opt_t *g;                                               \
    /*VALUE info = lp->info;*/                                  \
                                                                \
    INIT_COUNTER(lp, i);                                        \
    INIT_PTR_ELM(lp, 0, p1, s1, idx1, e1);                      \
    /*Data_Get_Struct(info,seq_opt_t,g);*/                      \
    g = lp->opt_ptr;                                            \
beg  = g->beg;                                                  \
    step = g->step;                                             \
    c    = g->count;                                            \
    if (idx1) {                                                 \
        for (; i--;) {                                          \
            x = beg + step * c++;                               \
            STORE_INT(p1+*idx1,e1,x);                           \
            idx1++;                                             \
        }                                                       \
    } else {                                                    \
        for (; i--;) {                                          \
            x = beg + step * c++;                               \
            STORE_INT(p1,e1,x);                                 \
            p1 += s1;                                           \
        }                                                       \
    }                                                           \
    g->count = c;                                               \
}                                                               \
static VALUE                                                    \
 nary_##intname##_seq(int argc, VALUE *args, VALUE self)        \
{                                                               \
    /*volatile VALUE opt;*/                                     \
    seq_opt_t *g;                                               \
    VALUE vary, vbeg=Qnil, vstep=Qnil;                          \
    ndfunc_t *func;                                             \
                                                                \
    func = ndfunc_alloc(intname##_seq, FULL_LOOP, 1, 0, Qnil);  \
    g = ALLOCA_N(seq_opt_t,1);                                  \
    g->beg = 0;                                                 \
    g->step = 1;                                                \
    g->count = 0;                                               \
    rb_scan_args(argc, args, "02", &vbeg, &vstep);              \
    if (vbeg!=Qnil) {g->beg = NUM2DBL(vbeg);}                   \
    if (vstep!=Qnil) {g->step = NUM2DBL(vstep);}                \
    /*opt = Data_Wrap_Struct(rb_cData,0,0,g);*/                 \
    ndloop_do3(func, g, 1, self);                               \
    ndfunc_free(func);                                          \
    return self;                                                \
}


#define DEF_FILL_INT_WITH_NUM(tpname, type, operation)          \
static void                                                     \
 tpname##_fill_loop(na_loop_t *const lp)                       \
{                                                               \
    size_t   i;                                                 \
    char    *p1;                                                \
    size_t   e1;                                                \
    ssize_t  s1;                                                \
    ssize_t *idx1;                                              \
    VALUE    x = *(VALUE*)(lp->opt_ptr);                       \
    type     y;                                                 \
    INIT_COUNTER(lp, i);                                       \
    INIT_PTR_ELM(lp, 0, p1, s1, idx1, e1);                     \
    {operation;}                                                \
    if (idx1) {                                                 \
        for (; i--;) {                                          \
            STORE_INT(p1+*idx1,e1,y);                           \
            idx1++;                                             \
        }                                                       \
    } else {                                                    \
        for (; i--;) {                                          \
            STORE_INT(p1,e1,y);                                 \
            p1 += s1;                                           \
        }                                                       \
    }                                                           \
}                                                               \
 static VALUE                                                   \
 nary_##tpname##_fill(VALUE self, volatile VALUE val)           \
 {                                                              \
    ndfunc_t *func;                                             \
    func = ndfunc_alloc(tpname##_fill_loop, FULL_LOOP,          \
                        1, 0, Qnil);                            \
    ndloop_do3(func, &val, 2, self);                            \
    ndfunc_free(func);                                          \
    return self;                                                \
}


#define DEF_RAND_INT(tpname,type,genrand,genbits,issigned)      \
static void                                                     \
 tpname##_rand_loop(na_loop_t *const lp)                       \
{                                                       \
    size_t   i;                                         \
    char    *p1;                                        \
    size_t   e1;                                        \
    ssize_t  s1;                                        \
    ssize_t *idx1;                                      \
    type     max, x;                                    \
    int      shift;                                     \
    rand_opt_t *g;                                      \
    /*VALUE    opt = lp->info;*/                       \
    /*Data_Get_Struct(opt,rand_opt_t,g);*/              \
    g = (rand_opt_t*)(lp->opt_ptr);                    \
    max   = g->max;                                     \
    shift = g->shift;                                   \
    INIT_COUNTER(lp, i);                               \
    INIT_PTR_ELM(lp, 0, p1, s1, idx1, e1);             \
    if (idx1) {                                         \
        for (; i--;) {                                  \
            do {x=genrand()>>shift;}                    \
            while (x>max);                              \
            STORE_INT(p1+*idx1,e1,x);                   \
            idx1++;                                     \
        }                                               \
    } else {                                            \
        for (; i--;) {                                  \
            do {x=genrand()>>shift;}                    \
            while (x>max);                              \
            STORE_INT(p1,e1,x);                         \
            p1 += s1;                                   \
        }                                               \
    }                                                   \
}                                                                       \
static VALUE                                                            \
 nary_##tpname##_rand(int argc, VALUE *args, VALUE self)                \
{                                                                       \
    VALUE opt, vmax=Qnil;                                               \
    ndfunc_t *func;                                                     \
    rand_opt_t *g;                                                      \
    u_int64_t max;                                                      \
    int bitsize;                                                        \
                                                                        \
    bitsize = FIX2INT(rb_const_get(CLASS_OF(self),id_element_bit_size));\
    bitsize -= issigned;                                                \
                                                                        \
    g = ALLOCA_N(rand_opt_t,1);                                         \
    g->max   = ((u_int64_t)1<<(bitsize))-1;                             \
    g->shift = (genbits)-(bitsize);                                     \
    g->sign  = 1;                                                       \
                                                                        \
    rb_scan_args(argc, args, "01", &vmax);                              \
    if (!NIL_P(vmax)) {                                                 \
        if (RTEST(rb_funcall(vmax,'<',1,INT2FIX(1)))) {                 \
            rb_raise(rb_eArgError,"rand-max must be positive");         \
        }                                                               \
        max = NUM2UINT64(vmax);                                         \
        if (max>g->max) {                                               \
            rb_raise(rb_eArgError,"rand-max is too big");               \
        }                                                               \
        g->max = max;                                                   \
        g->shift = (genbits)-n_bits(max);                               \
    }                                                                   \
    /*opt = Data_Wrap_Struct(rb_cData,0,0,g);*/                         \
                                                                        \
    func = ndfunc_alloc(tpname##_rand_loop, FULL_LOOP, 1, 0, Qnil); \
    ndloop_do3(func, g, 1, self);                                       \
    ndfunc_free(func);                                                  \
    return self;                                                        \
}


#define DEF_FORMAT_INT(tpname,type,elmsz,strsz,fmt)     \
static VALUE                                    \
format_##tpname(VALUE f, void *x)               \
{                                               \
    type y;                                     \
    LOAD_INT(x,elmsz,y);                        \
    if (NIL_P(f)) {                             \
        char s[strsz];                          \
        int n;                                  \
        n = sprintf(s,fmt,y);                   \
        return rb_str_new(s,n);                 \
    }                                           \
    return rb_funcall(f, '%', 1, INT2NUM(y));   \
}
