/*
  template_data.h
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2011 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/


#define DEF_BINARY(tpname, tpclass, type, opname, operation)            \
static void                                                             \
 iter_##tpname##_##opname(na_loop_t *const lp)                          \
{                                                                       \
    size_t  i;                                                          \
    char    *p1, *p2, *p3;                                              \
    ssize_t s1, s2, s3;                                                 \
    size_t  *idx1, *idx2, *idx3;                                        \
    type    x, y;                                                       \
    INIT_COUNTER(lp, i);                                                \
    INIT_PTR(lp, 0, p1, s1, idx1);                                      \
    INIT_PTR(lp, 1, p2, s2, idx2);                                      \
    INIT_PTR(lp, 2, p3, s3, idx3);                                      \
    if (idx1||idx2||idx3) {                                             \
        for (; i--;) {                                                  \
            LOAD_DATA_STEP(p1, s1, idx1, type, x);                      \
            LOAD_DATA_STEP(p2, s2, idx2, type, y);                      \
            operation;                                                  \
            STORE_DATA_STEP(p3, s3, idx3, type, x);                     \
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
    ndfunc_t *func;                                                     \
    VALUE v;                                                            \
    func = ndfunc_alloc(iter_##tpname##_##opname, FULL_LOOP,            \
                        2, 1, tpclass, tpclass, tpclass);               \
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
}                                                                       \


// ---------------------------------------------------------------------

#define DEF_BINARY_DI(tpname, tpclass, type,                            \
                      intname, intclass, inttype, opname, operation)    \
static void                                                             \
 iter_##tpname##_##opname##_##intname(na_loop_t *const lp)             \
{                                                                       \
    size_t  i;                                                          \
    char   *p1, *p2, *p3;                                               \
    ssize_t s1, s2, s3;                                                 \
    size_t  e2;                                                         \
    ssize_t *idx1, *idx2, *idx3;                                        \
    type    x;                                                          \
    inttype y;                                                          \
    INIT_COUNTER(lp, i);                                               \
    INIT_PTR(lp, 0, p1, s1, idx1);                                     \
    INIT_PTR_ELM(lp, 1, p2, s2, idx2, e2);                             \
    INIT_PTR(lp, 2, p3, s3, idx3);                                     \
    if (idx1||idx2||idx3) {                                             \
        for (; i--;) {                                                  \
            LOAD_DATA_STEP(p1, s1, idx1, type, x);                      \
            LOAD_INT_STEP(p2, s2, idx2, e2, inttype, y);                \
            operation;                                                  \
            STORE_DATA_STEP(p3, s3, idx3, type, x);                     \
        }                                                               \
    } else {                                                            \
        for (; i--;) {                                                  \
            x = *(type*)p1;                                             \
            p1+=s1;                                                     \
            LOAD_INT(p2,e2,y);                                          \
            p2+=s2;                                                     \
            operation;                                                  \
            *(type*)p3 = x;                                             \
            p3+=s3;                                                     \
        }                                                               \
    }                                                                   \
}                                                                       \
static VALUE                                                            \
 nary_##tpname##_s_##opname##_##intname(VALUE mod, VALUE a1, VALUE a2)  \
{                                                                       \
    ndfunc_t *func;                                                     \
    VALUE v;                                                            \
    func = ndfunc_alloc(iter_##tpname##_##opname##_##intname, FULL_LOOP, \
                        2, 1, tpclass, intclass, tpclass);              \
    v = ndloop_do(func, 2, a1, a2);                                \
    ndfunc_free(func);                                                  \
    return v;                                                           \
}                                                                       \
static VALUE                                                            \
 nary_##tpname##_##opname##_##intname(VALUE a1, VALUE a2)               \
{                                                                       \
    VALUE klass;                                                        \
    klass = na_upcast(CLASS_OF(a1),CLASS_OF(a2));                       \
    if (klass==tpclass) {                                               \
        return nary_##tpname##_s_##opname##_##intname(tpclass,a1,a2);   \
    }                                                                   \
    return rb_funcall(klass,id_##opname,2,a1,a2);                       \
}                                                                       \

// ---------------------------------------------------------------------

#define DEF_UNARY(tpname, tpclass, type, opname, operation)             \
static void                                                             \
iter_##tpname##_##opname(na_loop_t *const lp)                         \
{                                                                       \
    size_t  i;                                                          \
    char   *p1, *p2;                                                    \
    ssize_t s1, s2;                                                     \
    ssize_t *idx1, *idx2;                                               \
    type    x;                                                          \
    INIT_COUNTER(lp, i);                                               \
    INIT_PTR(lp, 0, p1, s1, idx1);                                     \
    INIT_PTR(lp, 1, p2, s2, idx2);                                     \
    if (idx1||idx2) {                                                   \
        for (; i--;) {                                                  \
            LOAD_DATA_STEP(p1, s1, idx1, type, x);                      \
            operation;                                                  \
            STORE_DATA_STEP(p2, s2, idx2, type, x);                     \
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
 nary_##tpname##_##opname(VALUE a1)                                     \
{                                                                       \
    ndfunc_t *func;                                                     \
    VALUE v;                                                            \
    func = ndfunc_alloc(iter_##tpname##_##opname, FULL_LOOP,            \
                        1, 1, tpclass, tpclass);                        \
    v = ndloop_do(func, 1, a1);                                    \
    ndfunc_free(func);                                                  \
    return v;                                                           \
}

// ---------------------------------------------------------------------

#define DEF_UNARY2(tpname, tpclass, type, tpclass2, type2, opname, operation)\
static void                                                             \
 tpname##_##opname##_loop(na_loop_t *const lp)                         \
{                                                                       \
    size_t  i;                                                          \
    char   *p1, *p2;                                                    \
    ssize_t s1, s2;                                                     \
    ssize_t *idx1, *idx2;                                               \
    type    x;                                                          \
    type2   y;                                                          \
    INIT_COUNTER(lp, i);                                               \
    INIT_PTR(lp, 0, p1, s1, idx1);                                     \
    INIT_PTR(lp, 1, p2, s2, idx2);                                     \
    if (idx1||idx2) {                                                   \
        for (; i--;) {                                                  \
            LOAD_DATA_STEP(p1, s1, idx1, type, x);                      \
            operation;                                                  \
            STORE_DATA_STEP(p2, s2, idx2, type2, y);                    \
        }                                                               \
    } else {                                                            \
        for (; i--;) {                                                  \
            x = *(type*)p1;                                             \
            p1+=s1;                                                     \
            operation;                                                  \
            *(type2*)p2 = y;                                            \
            p2+=s2;                                                     \
        }                                                               \
    }                                                                   \
}                                                                       \
static VALUE                                                            \
 nary_##tpname##_##opname(VALUE a1)                                     \
{                                                                       \
    ndfunc_t *func;                                                     \
    VALUE v;                                                            \
    func = ndfunc_alloc(tpname##_##opname##_loop, FULL_LOOP,            \
                        1, 1, tpclass, tpclass2);                       \
    v = ndloop_do(func, 1, a1);                                    \
    ndfunc_free(func);                                                  \
    return v;                                                           \
}

// ---------------------------------------------------------------------

#define DEF_SET2(tpname, tpclass, type, tpclass2, type2, opname, operation)\
static void                                                             \
 tpname##_##opname##_loop(na_loop_t *const lp)                         \
{                                                                       \
    size_t  i;                                                          \
    char   *p1, *p2;                                                    \
    ssize_t s1, s2;                                                     \
    ssize_t *idx1, *idx2;                                               \
    type    x, *q1;                                                     \
    type2   y;                                                          \
    INIT_COUNTER(lp, i);                                               \
    INIT_PTR(lp, 0, p1, s1, idx1);                                     \
    INIT_PTR(lp, 1, p2, s2, idx2);                                     \
    if (idx1||idx2) {                                                   \
        for (; i--;) {                                                  \
            LOAD_DATA_TEMP(q1, p1, s1, idx1, type, x);                  \
            LOAD_DATA_STEP(p2, s2, idx2, type2, y);                     \
            operation;                                                  \
            STORE_DATA(q1, type, x);                                    \
        }                                                               \
    } else {                                                            \
        for (; i--;) {                                                  \
            x = *(type*)p1;                                             \
            y = *(type2*)p2;                                            \
            p2+=s2;                                                     \
            operation;                                                  \
            *(type*)p1 = x;                                             \
            p1+=s1;                                                     \
        }                                                               \
    }                                                                   \
}                                                                       \
static VALUE                                                            \
 nary_##tpname##_##opname(VALUE a1, VALUE a2)                           \
{                                                                       \
    ndfunc_t *func;                                                     \
    func = ndfunc_alloc(tpname##_##opname##_loop, FULL_LOOP,            \
                        2, 0, Qnil, tpclass2);                          \
    ndloop_do(func, 2, a1, a2);                                    \
    ndfunc_free(func);                                                  \
    return a2;                                                          \
}

// ---------------------------------------------------------------------

#define DEF_UNARY_S(tpname, tpclass, type, opname, operation)           \
static void                                                             \
 tpname##_##opname##_loop(na_loop_t *const lp)                         \
{                                                                       \
    size_t  i;                                                          \
    char   *p1, *p2;                                                    \
    ssize_t s1, s2;                                                     \
    ssize_t *idx1, *idx2;                                               \
    type    x;                                                          \
    INIT_COUNTER(lp, i);                                               \
    INIT_PTR(lp, 0, p1, s1, idx1);                                     \
    INIT_PTR(lp, 1, p2, s2, idx2);                                     \
    if (idx1||idx2) {                                                   \
        for (; i--;) {                                                  \
            LOAD_DATA_STEP(p1, s1, idx1, type, x);                      \
            operation;                                                  \
            STORE_DATA_STEP(p2, s2, idx2, type, x);                     \
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
 nary_##tpname##_s_##opname(VALUE self, VALUE a1)                       \
{                                                                       \
    ndfunc_t *func;                                                     \
    VALUE v;                                                            \
    func = ndfunc_alloc(tpname##_##opname##_loop, FULL_LOOP,            \
                        1, 1, tpclass, tpclass);                        \
    v = ndloop_do(func, 1, a1);                                    \
    ndfunc_free(func);                                                  \
    return v;                                                           \
}

// ---------------------------------------------------------------------

#define DEF_CMP_BINARY(tpname, tpclass, type, opname, operation)   \
static void                                                             \
 iter_##tpname##_##opname(na_loop_t *const lp)                         \
{                                                                       \
    size_t  i;                                                          \
    char   *p1, *p2;                                                    \
    BIT_DIGIT *a3;                                                      \
    size_t  p3;                                                         \
    ssize_t s1, s2, s3;                                                 \
    ssize_t *idx1, *idx2, *idx3;                                        \
    type    x, y;                                                       \
    BIT_DIGIT b;                                                        \
    INIT_COUNTER(lp, i);                                               \
    INIT_PTR(lp, 0, p1, s1, idx1);                                     \
    INIT_PTR(lp, 1, p2, s2, idx2);                                     \
    INIT_PTR_BIT(lp, 2, a3, p3, s3, idx3);                             \
    if (idx1||idx2||idx3) {                                             \
        for (; i--;) {                                                  \
            LOAD_DATA_STEP(p1, s1, idx1, type, x);                      \
            LOAD_DATA_STEP(p2, s2, idx2, type, y);                      \
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
 nary_##tpname##_s_##opname(VALUE mod, VALUE a1, VALUE a2)              \
{                                                                       \
    ndfunc_t *func;                                                     \
    VALUE v;                                                            \
    func = ndfunc_alloc(iter_##tpname##_##opname, FULL_LOOP,            \
                        2, 1, tpclass, tpclass, cBit);                  \
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

// ---------------------------------------------------------------------

#define DEF_CMP_UNARY(tpname, tpclass, type, opname, operation) \
static void                                                     \
 tpname##_##opname##_loop(na_loop_t *const lp)                 \
{                                                               \
    size_t   i;                                                 \
    char    *p1;                                                \
    BIT_DIGIT *a2;                                              \
    size_t   p2;                                                \
    ssize_t  s1, s2;                                            \
    ssize_t *idx1, *idx2;                                       \
    type    x;                                                  \
    BIT_DIGIT b;                                                \
    INIT_COUNTER(lp, i);                                       \
    INIT_PTR(lp, 0, p1, s1, idx1);                             \
    INIT_PTR_BIT(lp, 1, a2, p2, s2, idx2);                     \
    if (idx1||idx2) {                                           \
        for (; i--;) {                                          \
            LOAD_DATA_STEP(p1, s1, idx1, type, x);              \
            b = (operation) ? 1:0;                              \
            STORE_BIT_STEP(a2, p2, s2, idx2, b);                \
        }                                                       \
    } else {                                                    \
        for (; i--;) {                                          \
            x = *(type*)p1;                                     \
            p1+=s1;                                             \
            b = (operation) ? 1:0;                              \
            STORE_BIT(a2,p2,b)                                  \
            p2+=s2;                                             \
        }                                                       \
    }                                                           \
}                                                               \
static VALUE                                                    \
 nary_##tpname##_##opname(VALUE a1)                             \
{                                                               \
    ndfunc_t *func;                                             \
    VALUE v;                                                    \
    func = ndfunc_alloc(tpname##_##opname##_loop, FULL_LOOP,    \
                        1, 1, tpclass, cBit);                   \
    v = ndloop_do(func, 1, a1);                            \
    return v;                                                   \
}

// ---------------------------------------------------------------------

#define DEF_ACCUM_UNARY(tpname, tpclass, type, opname, init, condition, operation)\
static void                                                             \
tpname##_##opname##_loop(na_loop_t *const lp)                          \
{                                                                       \
    size_t  i;                                                          \
    char   *p1, *p2;                                                    \
    type   *q2;                                                         \
    ssize_t s1, s2;                                                     \
    ssize_t *idx1, *idx2;                                               \
    type    x, y;                                                       \
                                                                        \
    INIT_COUNTER(lp, i);                                               \
    INIT_PTR(lp, 0, p1, s1, idx1);                                     \
    INIT_PTR(lp, 1, p2, s2, idx2);                                     \
    if (idx1||idx2) {                                                   \
        for (; i--;) {                                                  \
            LOAD_DATA_STEP(p1, s1, idx1, type, x);                      \
            LOAD_DATA_TEMP(q2, p2, s2, idx2, type, y);                  \
            if (condition) {                                            \
                {operation;}                                            \
                STORE_DATA(q2, type, x);                                \
            }                                                           \
        }                                                               \
    } else {                                                            \
        for (; i--;) {                                                  \
            x = *(type*)p1;                                             \
            p1+=s1;                                                     \
            y = *(type*)p2;                                             \
            if (condition) {                                            \
                {operation;}                                            \
                STORE_DATA(p2, type, x);                                \
            }                                                           \
            p2+=s2;                                                     \
        }                                                               \
    }                                                                   \
}                                                                       \
static VALUE                                                            \
 nary_##tpname##_##opname(int argc, VALUE *argv, VALUE self)            \
{                                                                       \
    VALUE v, accum;                                                     \
    ndfunc_t *func;                                                     \
    func = ndfunc_alloc(tpname##_##opname##_loop, FULL_LOOP,            \
                        2, 1, tpclass, sym_mark, tpclass);              \
    {init;};                                                            \
    accum = na_mark_dimension(argc, argv, self);                        \
    v = ndloop_do(func, 2, self, accum);                           \
    ndfunc_free(func);                                                  \
    return nary_##tpname##_extract(v);                                  \
}
// na_mark_dimension => na_parse_mark_arg
// ---------------------------------------------------------------------

#define DEF_INSPECT(tpname)                                             \
static VALUE                                                            \
 tpname##_inspect_element(char *ptr, size_t pos, VALUE fmt)             \
{                                                                       \
    return format_##tpname(fmt, ptr+pos);                               \
}                                                                       \
VALUE                                                                   \
 nary_##tpname##_inspect(VALUE ary)                                     \
{                                                                       \
    VALUE str = na_info_str(ary);                                       \
    ndloop_do_inspect(ary, str, tpname##_inspect_element, Qnil);   \
    return str;                                                         \
}

// ---------------------------------------------------------------------

#define DEF_FORMAT(tpname)                                      \
static void                                                     \
 iter_##tpname##_format(na_loop_t *const lp)                   \
{                                                               \
    size_t  i;                                                  \
    char   *p1, *p2;                                            \
    ssize_t s1, s2;                                             \
    ssize_t *idx1, *idx2;                                       \
    void *x;                                                    \
    VALUE y;                                                    \
    VALUE fmt = *(VALUE*)(lp->opt_ptr);                        \
    INIT_COUNTER(lp, i);                                       \
    INIT_PTR(lp, 0, p1, s1, idx1);                             \
    INIT_PTR(lp, 1, p2, s2, idx2);                             \
    for (; i--;) {                                              \
        LOAD_PTR_STEP(p1, s1, idx1, void, x);                   \
        y = format_##tpname(fmt, x);                            \
        STORE_DATA_STEP( p2, s2, idx2, VALUE, y );              \
    }                                                           \
}                                                               \
static VALUE                                                    \
 nary_##tpname##_format(int argc, VALUE *argv, VALUE self)      \
{                                                               \
     ndfunc_t *func;                                            \
     volatile VALUE v, fmt=Qnil;                                \
                                                                \
     rb_scan_args(argc, argv, "01", &fmt);                      \
     func = ndfunc_alloc(iter_##tpname##_format, FULL_LOOP,     \
                         1, 1, Qnil, cRObject);                 \
     v = ndloop_do3(func, &fmt, 1, self);                       \
     ndfunc_free(func);                                         \
     return v;                                                  \
}                                                               \
static void                                                     \
 iter_##tpname##_format_to_a(na_loop_t *const lp)              \
{                                                               \
    size_t  i;                                                  \
    char   *p1, *p2;                                            \
    ssize_t s1, s2;                                             \
    ssize_t *idx1, *idx2;                                       \
    void *x;                                                    \
    VALUE y;                                                    \
    volatile VALUE a;                                           \
    VALUE fmt = *(VALUE*)(lp->opt_ptr);                        \
    INIT_COUNTER(lp, i);                                       \
    INIT_PTR(lp, 0, p1, s1, idx1);                             \
    a = rb_ary_new2(i);                                         \
    rb_ary_push(lp->args[1].value, a);                         \
    for (; i--;) {                                              \
        LOAD_PTR_STEP(p1, s1, idx1, void, x);                   \
        y = format_##tpname(fmt, x);                            \
        rb_ary_push(a,y);                                       \
    }                                                           \
}                                                               \
static VALUE                                                    \
 nary_##tpname##_format_to_a(int argc, VALUE *argv, VALUE self) \
{                                                               \
     ndfunc_t *func;                                            \
     volatile VALUE v, fmt=Qnil;                                \
                                                                \
     rb_scan_args(argc, argv, "01", &fmt);                      \
     func = ndfunc_alloc(iter_##tpname##_format_to_a,FULL_LOOP, \
                         1, 1, Qnil, rb_cArray);                \
     v = ndloop_cast_narray_to_rarray(func, self, fmt);         \
     ndfunc_free(func);                                         \
     return v;                                                  \
}

// ---------------------------------------------------------------------

#define DEF_SEQ_DATA(tpname, tpclass, type, operation)          \
static void                                                     \
 tpname##_seq_loop(na_loop_t *const lp)                         \
{                                                               \
    size_t i;                                                   \
    char   *p1;                                                 \
    ssize_t  s1;                                                \
    ssize_t *idx1;                                              \
    double  x, beg, step;                                       \
    type    y;                                                  \
    size_t  c;                                                  \
    seq_opt_t *g;                                               \
    /*VALUE info;*/                                             \
                                                                \
    INIT_COUNTER(lp, i);                                        \
    INIT_PTR(lp, 0, p1, s1, idx1);                              \
    /* info */                                                  \
    /*info = lp->info;*/                                        \
    /*Data_Get_Struct(info,seq_opt_t,g);*/                      \
    g = (seq_opt_t*)(lp->opt_ptr);                              \
    beg  = g->beg;                                              \
    step = g->step;                                             \
    c    = g->count;                                            \
    if (idx1) {                                                 \
        for (; i--;) {                                          \
            x = beg + step * c++;                               \
            {operation;}                                        \
            *(type*)(p1+*idx1) = y;                             \
            idx1++;                                             \
        }                                                       \
    } else {                                                    \
        for (; i--;) {                                          \
            x = beg + step * c++;                               \
            {operation;}                                        \
            *(type*)(p1) = y;                                   \
            p1 += s1;                                           \
        }                                                       \
    }                                                           \
    g->count = c;                                               \
}                                                               \
static VALUE                                                    \
 nary_##tpname##_seq(int argc, VALUE *args, VALUE self)         \
{                                                               \
    volatile VALUE opt;                                         \
    seq_opt_t *g;                                               \
    VALUE vary, vbeg=Qnil, vstep=Qnil;                          \
    ndfunc_t *func;                                             \
                                                                \
    func = ndfunc_alloc(tpname##_seq_loop, FULL_LOOP,           \
                        1, 0, Qnil);                            \
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

// ---------------------------------------------------------------------

#define DEF_RAND_DATA(tpname, tpclass, type, opname, operation) \
static void                                                     \
 tpname##_##opname##_loop(na_loop_t *const lp)                 \
{                                                               \
    size_t   i;                                                 \
    char    *p1;                                                \
    ssize_t  s1;                                                \
    ssize_t *idx1;                                              \
    type     y;                                                 \
                                                                \
    INIT_COUNTER(lp, i);                                       \
    INIT_PTR(lp, 0, p1, s1, idx1);                             \
    if (idx1) {                                                 \
        for (; i--;) {                                          \
            {operation};                                        \
            *(type*)(p1+*idx1) = y;                             \
            idx1++;                                             \
        }                                                       \
    } else {                                                    \
        for (; i--;) {                                          \
            {operation};                                        \
            *(type*)(p1) = y;                                   \
            p1+=s1;                                             \
        }                                                       \
    }                                                           \
}                                                               \
static VALUE                                                    \
 nary_##tpname##_##opname(VALUE self)                           \
{                                                               \
    ndfunc_t *func;                                             \
    func = ndfunc_alloc(tpname##_##opname##_loop, FULL_LOOP,    \
                        1, 0, Qnil);                            \
    ndloop_do(func, 1, self);                              \
    ndfunc_free(func);                                          \
    return self;                                                \
}

// ---------------------------------------------------------------------

#define DEF_RAND_NORM_DATA(tpname, tpclass, type, operation)    \
static void                                                     \
 tpname##_rand_norm_loop(na_loop_t *const lp)                  \
{                                                               \
    size_t   i;                                                 \
    char    *p1;                                                \
    ssize_t  s1;                                                \
    ssize_t *idx1;                                              \
    type     y;                                                 \
    type     a[2];                                              \
                                                                \
    INIT_COUNTER(lp, i);                                       \
    INIT_PTR(lp, 0, p1, s1, idx1);                             \
    if (idx1) {                                                 \
        for (; i>1; i-=2) {                                     \
            {operation;}                                        \
            *(type*)(p1+*idx1)     = a[0];                      \
            *(type*)(p1+*(idx1+1)) = a[1];                      \
            idx1 += 2;                                          \
        }                                                       \
        if (i>0) {                                              \
            {operation};                                        \
            *(type*)(p1+*idx1) = a[0];                          \
        }                                                       \
    } else {                                                    \
        for (; i>1; i-=2) {                                     \
            {operation;}                                        \
            *(type*)(p1)    = a[0];                             \
            *(type*)(p1+s1) = a[1];                             \
            p1 += s1*2;                                         \
        }                                                       \
        if (i>0) {                                              \
            {operation;}                                        \
            *(type*)(p1) = a[0];                                \
        }                                                       \
    }                                                           \
}                                                               \
static VALUE                                                    \
 nary_##tpname##_rand_norm(VALUE self)                          \
{                                                               \
    ndfunc_t *func;                                             \
    func = ndfunc_alloc(tpname##_rand_norm_loop, FULL_LOOP,     \
                        1, 0, Qnil);                            \
    ndloop_do(func, 1, self);                              \
    ndfunc_free(func);                                          \
    return self;                                                \
}

// ---------------------------------------------------------------------

#define DEF_FILL_DATA_WITH_NUM(tpname, type, tpclass, operation)\
static void                                                     \
 iter_##tpname##_fill(na_loop_t *const lp)                      \
{                                                               \
    size_t   i;                                                 \
    char    *p1;                                                \
    ssize_t  s1;                                                \
    ssize_t *idx1;                                              \
    VALUE    x = *(VALUE*)(lp->opt_ptr);                        \
    type     y;                                                 \
    INIT_COUNTER(lp, i);                                        \
    INIT_PTR(lp, 0, p1, s1, idx1);                              \
    {operation;}                                                \
    if (idx1) {                                                 \
        for (; i--;) {*(type*)(p1+*(idx1++)) = y;}              \
    } else {                                                    \
        for (; i--;) {*(type*)(p1) = y; p1+=s1;}                \
    }                                                           \
}                                                               \
 static VALUE                                                   \
 nary_##tpname##_fill(VALUE self, volatile VALUE val)           \
{                                                               \
    ndfunc_t *func;                                             \
    func = ndfunc_alloc(iter_##tpname##_fill, FULL_LOOP,        \
                        1, 0, Qnil);                            \
    ndloop_do3(func, &val, 1, self);                            \
    ndfunc_free(func);                                          \
    return self;                                                \
}

// ---------------------------------------------------------------------

#define POW_DATA_INT(type)                      \
static type pow_##type##_int(type x, int p)     \
{                                               \
    type r=1;                                   \
    switch(p) {                                 \
    case 2: return x*x;                         \
    case 3: return x*x*x;                       \
    case 4: x=x*x; return x*x;                  \
    case 1: return x;                           \
    case 0: return 1;                           \
    }                                           \
    if (p<0)  return 1/pow_##type##_int(x,-p);  \
    if (p>64) return pow(x,p);                  \
    while (p) {                                 \
        if ((p%2) == 1) r *= x;                 \
        x *= x;                                 \
        p /= 2;                                 \
    }                                           \
    return r;                                   \
}
