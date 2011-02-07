/*
  template_cast.h
  Numerical Array Extension for Ruby
    (C) Copyright 1999-2011 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/

#define CAST_NUM_TO_DATA( tpname, type, tpclass, operation ) \
static VALUE                                                 \
 nary_##tpname##_new_dim0( type x )                                  \
{                                                                    \
    narray_t *na;                                                    \
    VALUE v;                                                         \
    type *ptr;                                                       \
                                                                     \
    v = rb_narray_new( tpclass, 0, NULL );                              \
    GetNArray(v,na);                                                    \
    ptr = (type*)(char*)na_get_pointer_for_write(v);                    \
    *ptr = x;                                                           \
    na_release_lock(v);                                                 \
    return v;                                                           \
}                                                                       \
static VALUE                                                            \
 nary_cast_numeric_to_##tpname( VALUE x )                               \
{                                                                       \
    type y;                                                             \
    {operation;}                                                        \
    return nary_##tpname##_new_dim0( y );                               \
}

//#define CAST_NUMERIC_TO_DATA(tpname, type, tpclass, operation)          \
//static VALUE                                                            \
// nary_cast_numeric_to_##tpname(VALUE x)                                 \
//{                                                                       \
//    type y;                                                             \
//    narray_t *na;                                                       \
//    VALUE v;                                                            \
//    type *ptr;                                                          \
//                                                                        \
//    operation;                                                          \
//                                                                        \
//    v = rb_narray_new(tpclass, 0, NULL);                                \
//    ptr = (type*)na_get_pointer_for_write(v);                           \
//    *ptr = y;                                                           \
//    na_release_lock(v);                                                 \
//    return v;                                                           \
//}
//
// ---------------------------------------------------------------------


#define CAST_DATA_TO_VALUE(tp1name, tp1, tp1class, tp2name, tp2class, operation) \
void                                                                    \
 iter_##tp1name##_to_##tp2name(na_loop_t *const itr)                    \
{                                                                       \
    size_t i, s1, s2;                                                   \
    char   *p1, *p2;                                                    \
    size_t *idx1, *idx2;                                                \
    tp1 x;                                                              \
    volatile VALUE y;                                                   \
                                                                        \
    INIT_COUNTER(itr, i);                                               \
    INIT_PTR(itr, 0, p1, s1, idx1);                                     \
    INIT_PTR(itr, 1, p2, s2, idx2);                                     \
    for (; i--;) {                                                      \
        LOAD_DATA_STEP(p1, s1, idx1, tp1, x);                           \
        operation;                                                      \
        STORE_DATA_STEP(p2, s2, idx2, VALUE, y);                        \
    }                                                                   \
}                                                                       \
static VALUE                                                            \
 nary_cast_##tp1name##_to_##tp2name(VALUE self)                         \
{                                                                       \
    VALUE v;                                                            \
    ndfunc_t *func ;                                                    \
                                                                        \
    func = ndfunc_alloc(iter_##tp1name##_to_##tp2name, FULL_LOOP,       \
                        1, 1, tp1class, tp2class);                      \
    v = ndloop_do(func, 1, self);                                  \
    ndfunc_free(func);                                                  \
    return v;                                                           \
}

// ---------------------------------------------------------------------

#define CAST_DATA_TO_ARRAY(tpname, type, tpclass, operation)            \
void                                                                    \
 iter_##tpname##_to_rarray(na_loop_t *const itr)                        \
{                                                                       \
    size_t i, s1, s2;                                                   \
    char *p1, *p2;                                                      \
    size_t *idx1, *idx2;                                                \
    type x;                                                             \
    volatile VALUE a, y;                                                \
                                                                        \
    INIT_COUNTER(itr, i);                                               \
    INIT_PTR(itr, 0, p1, s1, idx1);                                     \
    /*INIT_PTR(itr, 1, p2, s2, idx2);*/                                 \
    a = rb_ary_new2(i);                                                 \
    rb_ary_push(itr->args[1].value, a);                                 \
    for (; i--;) {                                                      \
        LOAD_DATA_STEP(p1, s1, idx1, type, x);                          \
        operation;                                                      \
        rb_ary_push(a,y);                                               \
        /*STORE_DATA_STEP(p2, s2, idx2, VALUE, y);*/                    \
    }                                                                   \
}                                                                       \
static VALUE                                                            \
 nary_cast_##tpname##_to_array(VALUE self)                              \
{                                                                       \
    VALUE v;                                                            \
    ndfunc_t *func ;                                                    \
                                                                        \
    func = ndfunc_alloc(iter_##tpname##_to_rarray, FULL_LOOP,           \
                        1, 1, Qnil, rb_cArray);                         \
    v = ndloop_cast_narray_to_rarray(func, self, Qnil);                 \
    ndfunc_free(func);                                                  \
    return v;                                                           \
}

// ---------------------------------------------------------------------


#define CAST_ARRAY_TO_DATA(tpname, type, tpclass, num2dat, dbl2dat, setdefault)       \
static void                                                             \
 iter_cast_rarray_to_##tpname(na_loop_t *const lp)                      \
{                                                                       \
    size_t i, n;                                                        \
    size_t i1, n1;                                                      \
    VALUE  v1, *ptr;                                                    \
    char   *p2;                                                         \
    size_t s2, *idx2;                                                   \
    VALUE  x;                                                           \
    double y;                                                           \
    type   z;                                                           \
    size_t len, c;                                                      \
    double beg, step;                                                   \
                                                                        \
    INIT_COUNTER(lp, n);                                                \
    v1 = lp->args[0].value;                                             \
    INIT_PTR(lp, 1, p2, s2, idx2);                                      \
                                                                        \
    switch(TYPE(v1)) {                                                  \
    case T_ARRAY:                                                       \
        n1 = RARRAY_LEN(v1);                                            \
        ptr = RARRAY_PTR(v1);                                           \
        break;                                                          \
    case T_NIL:                                                         \
        n1 = 0;                                                         \
        break;                                                          \
    default:                                                            \
        n1 = 1;                                                         \
        ptr = &v1;                                                      \
    }                                                                   \
    for (i=i1=0; i1<n1 && i<n; i++,i1++) {                              \
        x = ptr[i1];                                                    \
        if (rb_obj_is_kind_of(x, rb_cRange) || rb_obj_is_kind_of(x, na_cStep)) { \
            nary_step_sequence(x,&len,&beg,&step);                      \
            for (c=0; c<len && i<n; c++,i++) {                          \
                y = beg + step * c;                                     \
                {dbl2dat;}                                              \
                STORE_DATA_STEP(p2, s2, idx2, type, z);                 \
            }                                                           \
        }                                                               \
        else if (TYPE(x) != T_ARRAY) {                                  \
            if (x == Qnil) x = INT2FIX(0);                              \
            {num2dat;}                                                  \
            STORE_DATA_STEP(p2, s2, idx2, type, z);                     \
        }                                                               \
    }                                                                   \
    {setdefault;}                                                       \
    for (; i<n; i++) {                                                  \
        STORE_DATA_STEP(p2, s2, idx2, type, z);                         \
    }                                                                   \
}                                                                       \
static VALUE                                                            \
 nary_cast_array_to_##tpname(VALUE rary)                                \
{                                                                       \
    int nd;                                                             \
    size_t *shape;                                                      \
    VALUE tp, nary;                                                     \
    ndfunc_t *func;                                                     \
                                                                        \
    shape = na_mdarray_investigate(rary, &nd, &tp);                     \
    nary = rb_narray_new(tpclass, nd, shape);                           \
    na_alloc_data(nary);                                                \
    xfree(shape);                                                       \
    func = ndfunc_alloc(iter_cast_rarray_to_##tpname, FULL_LOOP,        \
                        2, 0, Qnil, rb_cArray);                         \
    ndloop_cast_rarray_to_narray(func, rary, nary);                     \
    ndfunc_free(func);                                                  \
    return nary;                                                        \
}

// ---------------------------------------------------------------------

#define CAST_INT_TO_NUM(intname, inttype, int2num)                \
static VALUE                                                      \
 nary_##intname##_extract(VALUE self)                             \
{                                                                 \
    VALUE klass;                                                  \
    char *ptr;                                                    \
    int esz;                                                      \
    inttype x;                                                    \
    narray_t *na;                                                 \
    GetNArray(self,na);                                           \
    if (na->ndim==0) {                                            \
        ptr = na_get_pointer_for_read(self) + na_get_offset(self);\
        klass = CLASS_OF(self);                                   \
        SET_INT_ELEMENT_SIZE(klass,esz);                          \
        LOAD_INT(ptr, esz, x);                                    \
        return int2num(x);                                        \
    }                                                             \
    return self;                                                  \
}

// ---------------------------------------------------------------------

#define CAST_NUM_TO_INT(intname, inttype, num2int)              \
static VALUE                                                    \
 nary_##intname##_new_dim0(VALUE klass, inttype x)              \
{                                                               \
    narray_t *na;                                               \
    VALUE v;                                                    \
    char *ptr;                                                  \
    int esz;                                                    \
    v = rb_narray_new(klass, 0, NULL);                          \
    GetNArray(v,na);                                            \
    ptr = na_get_pointer_for_write(v) + na_get_offset(v);       \
    SET_INT_ELEMENT_SIZE(klass, esz);                           \
    STORE_INT(ptr, esz, x);                                     \
    return v;                                                   \
}                                                               \
static VALUE                                                    \
 nary_cast_numeric_to_##intname(VALUE klass, VALUE val)         \
{                                                               \
    return nary_##intname##_new_dim0(klass, num2int(val));      \
}

// ---------------------------------------------------------------------
// DFloat --> Int32..Int8

#define CAST_DATA_TO_INT(dataname, datatype, intname, inttype)      \
static void                                                         \
 dataname##_to_##intname(na_loop_t *const itr)                      \
{                                                                   \
    size_t  i, s1, s2;                                              \
    char   *p1, *p2;                                                \
    size_t *idx1, *idx2;                                            \
    int     e2;                                                     \
    datatype x;                                                     \
    inttype  y;                                                     \
                                                                    \
    INIT_COUNTER(itr, i);                                           \
    INIT_PTR(itr, 0, p1, s1, idx1);                                 \
    INIT_PTR_ELM(itr, 1, p2, s2, idx2, e2);                         \
    if (idx1||idx2)                                                 \
        for (; i--;) {                                              \
            LOAD_DATA_STEP(p1, s2, idx1, datatype, x);              \
            y = x;                                                  \
            STORE_INT_STEP(p2, s2, idx2, e2, inttype, y);           \
        }                                                           \
    else                                                            \
        for (; i--;) {                                              \
            x = *(datatype*)p1;                                     \
            p1+=s1;                                                 \
            y = x;                                                  \
            STORE_INT(p2,e2,y);                                     \
            p2+=s2;                                                 \
        }                                                           \
}                                                                   \
static VALUE                                                        \
 nary_cast_##dataname##_to_##intname(VALUE klass, VALUE self)       \
{                                                                   \
    VALUE v;                                                        \
    ndfunc_t *func;                                                 \
    func = ndfunc_alloc(dataname##_to_##intname, FULL_LOOP,         \
                        1, 1, Qnil, klass);                         \
    v = ndloop_do(func, 1, self);                              \
    ndfunc_free(func);                                              \
    return v;                                                       \
}

// ---------------------------------------------------------------------

// Int -> DFloat
#define CAST_INT_TO_DATA(intname, inttype, dataname, datatype, dataclass, operation) \
static void                                                         \
 intname##_cast_to_##dataname(na_loop_t *const itr)                 \
{                                                                   \
    size_t  i, s1, s2;                                              \
    char   *p1, *p2;                                                \
    size_t *idx1, *idx2;                                            \
    int     e1;                                                     \
    inttype x;                                                      \
    datatype  y;                                                    \
    INIT_COUNTER(itr, i);                                           \
    INIT_PTR_ELM(itr, 0, p1, s1, idx1, e1);                         \
    INIT_PTR(itr, 1, p2, s2, idx2);                                 \
    for (; i--;) {                                                  \
        LOAD_INT_STEP(p1, s1, idx1, e1, inttype, x);                \
        operation;                                                  \
        STORE_DATA_STEP(p2, s2, idx2, datatype, y);                 \
    }                                                               \
}                                                                   \
VALUE                                                               \
nary_cast_##intname##_to_##dataname(VALUE self)                     \
{                                                                   \
    VALUE v;                                                        \
    ndfunc_t *func;                                                 \
    func = ndfunc_alloc(intname##_cast_to_##dataname,               \
                        FULL_LOOP, 1, 1, Qnil, dataclass);          \
    v = ndloop_do(func, 1, self);                              \
    ndfunc_free(func);                                              \
    return v;                                                       \
}

// ---------------------------------------------------------------------

#define CAST_INT_TO_ARRAY(tpname, type, int2num)                \
void                                                            \
 tpname##_cast_to_array(na_loop_t *const itr)                   \
{                                                               \
    size_t i, s1, s2;                                           \
    char   *p1, *p2;                                            \
    size_t *idx1, *idx2;                                        \
    int     e1;                                                 \
    type    x;                                                  \
    volatile VALUE a, y;                                        \
    INIT_COUNTER(itr, i);                                       \
    INIT_PTR_ELM(itr, 0, p1, s1, idx1, e1);                     \
    a = rb_ary_new2(i);                                         \
    rb_ary_push(itr->args[1].value, a);                         \
    for (; i--;) {                                              \
        LOAD_INT_STEP(p1, s1, idx1, e1, type, x);               \
        y = int2num(x);                                         \
        rb_ary_push(a,y);                                       \
    }                                                           \
}                                                               \
 static VALUE                                                   \
 nary_cast_##tpname##_to_array(VALUE self)                      \
{                                                               \
    VALUE v;                                                    \
    ndfunc_t *func;                                             \
    func = ndfunc_alloc(tpname##_cast_to_array, FULL_LOOP,      \
                        1, 1, Qnil, rb_cArray);                 \
    v = ndloop_cast_narray_to_rarray(func, self, Qnil);         \
    ndfunc_free(func);                                          \
    return v;                                                   \
}

// ---------------------------------------------------------------------

#define CAST_ARRAY_TO_INT(tpname, type, num2int)                        \
static void                                                             \
 iter_cast_rarray_to_##tpname(na_loop_t *const lp)                      \
{                                                                       \
    size_t i, n;                                                        \
    size_t i1, n1;                                                      \
    VALUE  v1, *ptr;                                                    \
    char   *p2;                                                         \
    size_t s2, *idx2;                                                   \
    int    e2;                                                          \
    VALUE  x;                                                           \
    type   y;                                                           \
    size_t len, c;                                                      \
    double beg, step;                                                   \
                                                                        \
    INIT_COUNTER(lp, n);                                                \
    v1 = lp->args[0].value;                                             \
    INIT_PTR_ELM(lp, 1, p2, s2, idx2, e2);                              \
                                                                        \
    switch(TYPE(v1)) {                                                  \
    case T_ARRAY:                                                       \
        n1 = RARRAY_LEN(v1);                                            \
        ptr = RARRAY_PTR(v1);                                           \
        break;                                                          \
    case T_NIL:                                                         \
        n1 = 0;                                                         \
        break;                                                          \
    default:                                                            \
        n1 = 1;                                                         \
        ptr = &v1;                                                      \
    }                                                                   \
    for (i=i1=0; i1<n1 && i<n; i++,i1++) {                              \
        x = ptr[i1];                                                    \
        if (rb_obj_is_kind_of(x, rb_cRange) || rb_obj_is_kind_of(x, na_cStep)) { \
            nary_step_sequence(x,&len,&beg,&step);                      \
            for (c=0; c<len && i<n; c++,i++) {                          \
                y = beg + step * c;                                     \
                STORE_INT_STEP(p2, s2, idx2, e2, type, y);              \
            }                                                           \
        }                                                               \
        else if (TYPE(x) != T_ARRAY) {                                  \
            if (x == Qnil) x = INT2FIX(0);                              \
            {y = num2int(x);}                                           \
            STORE_INT_STEP(p2, s2, idx2, e2, type, y);                  \
        }                                                               \
    }                                                                   \
    y = 0;                                                              \
    for (; i<n; i++) {                                                  \
        STORE_INT_STEP(p2, s2, idx2, e2, type, y);                      \
    }                                                                   \
}                                                                       \
static VALUE                                                            \
 nary_cast_array_to_##tpname(VALUE klass, VALUE rary)                   \
{                                                                       \
    int nd;                                                             \
    size_t *shape;                                                      \
    VALUE tp, nary;                                                     \
    ndfunc_t *func;                                                     \
                                                                        \
    shape = na_mdarray_investigate(rary, &nd, &tp);                     \
    nary = rb_narray_new(klass, nd, shape);                             \
    na_alloc_data(nary);                                                \
    xfree(shape);                                                       \
    func = ndfunc_alloc(iter_cast_rarray_to_##tpname, FULL_LOOP,        \
                        2, 0, Qnil, rb_cArray);                         \
    ndloop_cast_rarray_to_narray(func, rary, nary);                     \
    ndfunc_free(func);                                                  \
    return nary;                                                        \
}

