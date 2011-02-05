VALUE
nary_sort_bang(int argc, VALUE *argv, VALUE self, VALUE tpclass, na_iter_func_t *iter_sort)
{
    VALUE tpclass;
    VALUE     view, copy;
    ndfunc_t *func;
    int       ndim;
    narray_t *na;

    view = na_flatten_by_mark(argc, argv, self);
    GetNArray(view,na);
    ndim = na->ndim;
    if (ndim<1) return self;
    if (CLASS_OF(self)!=tpclass) {
        //copy = nary_cast_##tpname##_to_##tpname(tpclass,view);
        copy = rb_funcall(tpclass, rb_intern("cast"), 1, view);
    } else if (!na_check_ladder(view,-1)) {
        copy = na_copy(view);
    } else {
        copy = view;
    }
    func = ndfunc_alloc(iter_sort, NO_LOOP,1, 0, Qnil);
    func->args[0].dim = 1;
    ndfunc_execute(func, 1, copy);
    ndfunc_free(func);
    if (view!=copy) {
        na_store(view, copy);
    }
    return self;
}
