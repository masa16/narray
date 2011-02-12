static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t  i;
    char    *p1, *p2, *p3;
    ssize_t s1, s2, s3;
    size_t  *idx1, *idx2, *idx3;
    dtype    x, y;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    INIT_PTR(lp, 2, p3, s3, idx3);
    if (idx1||idx2||idx3) {
        for (; i--;) {
            LOAD_DATA_STEP(p1, s1, idx1, dtype, x);
            LOAD_DATA_STEP(p2, s2, idx2, dtype, y);
            x = m_pow(x,y);
            STORE_DATA_STEP(p3, s3, idx3, dtype, x);
        }
    } else {
        for (; i--;) {
            x = *(dtype*)p1;
            p1+=s1;
            y = *(dtype*)p2;
            p2+=s2;
            x = m_pow(x,y);
            *(dtype*)p3 = x;
            p3+=s3;
        }
    }
}

static dtype pow_<%=$tp%>_int(dtype x, int p)
{
    dtype r = m_one;
    switch(p) {
    case 2: return m_square(x);
    case 3: return m_mul(m_square(x),x);
    case 4: return x = m_square(x); return m_square(x);
    case 1: return x;
    case 0: return m_one;
    }
    if (p<0)  return m_inverse(pow_<%=tp%>_int(x,-p));
    if (p>64) return m_pow(x,m_from_real(p));
    while (p) {
        if ((p%2) == 1) r = m_mul(r,x);
        x = m_square(x);
        p /= 2;
    }
    return r;
}


static void
<%=c_iterator%>_int32(na_loop_t *const lp)
{
    size_t  i;
    char   *p1, *p2, *p3;
    ssize_t s1, s2, s3;
    size_t  e2;
    ssize_t *idx1, *idx2, *idx3;
    dtype    x;
    int32_t y;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR_ELM(lp, 1, p2, s2, idx2, e2);
    INIT_PTR(lp, 2, p3, s3, idx3);
    if (idx1||idx2||idx3) {
        for (; i--;) {
            LOAD_DATA_STEP(p1, s1, idx1, dtype, x);
            LOAD_INT_STEP(p2, s2, idx2, e2, int32_t, y);
            x = pow_<%=tp%>_int(x,y);
            STORE_DATA_STEP(p3, s3, idx3, dtype, x);
        }
    } else {
        for (; i--;) {
            x = *(dtype*)p1;
            p1+=s1;
            LOAD_INT(p2,e2,y);
            p2+=s2;
            x = pow_<%=tp%>_int(x,y);
            *(dtype*)p3 = x;
            p3+=s3;
        }
    }
}

static VALUE
<%=c_singleton_method%>(VALUE mod, VALUE a1, VALUE a2)
{
    ndfunc_t *func;
    VALUE v;
    if (FIXNUM_P(a2) || rb_obj_is_kind_of(a2,cInt32)) {
        func = ndfunc_alloc(<%=c_iterator%>_int32, FULL_LOOP,
                            2, 1, cT, cInt32, cT);
    } else {
        func = ndfunc_alloc(<%=c_iterator%>, FULL_LOOP,
                            2, 1, cT, cT, cT);
    }
    v = ndloop_do(func, 2, a1, a2);
    ndfunc_free(func);
    return v;
}

static VALUE
<%=c_instance_method%>(VALUE a1, VALUE a2)
{
    VALUE klass;
    klass = na_upcast(CLASS_OF(a1),CLASS_OF(a2));
    if (klass==cT) {
        return <%=c_singleton_method%>(cT,a1,a2);
    } else {
        return rb_funcall(klass,id_pow,2,a1,a2);
    }
}
