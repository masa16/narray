/*
  template_data.h
  Numerical Array Extention for Ruby
    (C) Copyright 1999-2007 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/


#define CAST_NUM_TO_DATA( tpname, type, tpclass, operation ) \
static VALUE						     \
 num_##tpname##_new_dim0( type x )				     \
{								     \
    narray_t *na;						     \
    VALUE v;							     \
    type *ptr;							     \
    								     \
    v = rb_narray_new( tpclass, 0, NULL );				\
    GetNArray(v,na);							\
    ptr = (type*)((char*)na_data_pointer_for_write(v) + na->offset);	\
    *ptr = x;								\
    return v;								\
}									\
static VALUE								\
 num_cast_numeric_to_##tpname( VALUE x )				\
{									\
    type y;								\
    {operation;}							\
    return num_##tpname##_new_dim0( y );				\
}


#define CAST_DATA_TO_VALUE( tp1name, tp1, tp1class, tp2name, tp2class, operation ) \
void								\
 cast_##tp1name##_to_##tp2name( na_iterator_t *const itr, VALUE info )	\
{									\
    size_t i, s1, s2;							\
    char   *p1, *p2;							\
    size_t *idx1, *idx2;						\
    tp1 x;								\
    volatile VALUE y;							\
    									\
    INIT_COUNTER( itr, i );						\
    INIT_PTR( itr, 0, p1, s1, idx1 );					\
    INIT_PTR( itr, 1, p2, s2, idx2 );					\
    for (; i--; ) {							\
	LOAD_DATA_STEP( p1, s1, idx1, tp1, x );				\
	operation;							\
	STORE_DATA_STEP( p2, s2, idx2, VALUE, y );			\
    }									\
}									\
static VALUE								\
 num_cast_##tp1name##_to_##tp2name( VALUE self )			\
{									\
    VALUE v;								\
    ndfunc_t *func ;							\
									\
    func = ndfunc_alloc( cast_##tp1name##_to_##tp2name, HAS_LOOP,	\
			 1, 1, tp1class, tp2class );			\
    v = ndfunc_execute( func, 1, self );				\
    ndfunc_free(func);							\
    return v;								\
}


#define CAST_DATA_TO_ARRAY( tpname, type, tpclass, operation )	\
void								\
cast_##tpname##_to_array( na_iterator_t *const itr, VALUE info )	\
{									\
    size_t i, s1, s2;							\
    char   *p1, *p2;							\
    size_t *idx1, *idx2;						\
    type x;								\
    volatile VALUE y;							\
    									\
    INIT_COUNTER( itr, i );						\
    INIT_PTR( itr, 0, p1, s1, idx1 );					\
    INIT_PTR( itr, 1, p2, s2, idx2 );					\
    for (; i--; ) {							\
	LOAD_DATA_STEP( p1, s1, idx1, type, x );			\
	operation;							\
	STORE_DATA_STEP( p2, s2, idx2, VALUE, y );			\
    }									\
}									\
static VALUE								\
num_cast_##tpname##_to_array( VALUE self )				\
{									\
    VALUE v;								\
    ndfunc_t *func ;							\
    									\
    func = ndfunc_alloc( cast_##tpname##_to_array, HAS_LOOP,		\
			 1, 1, Qnil, rb_cArray );			\
    v = ndfunc_execute_to_rarray( func, self, Qnil );			\
    ndfunc_free(func);							\
    return v;								\
}


#define CAST_ARRAY_TO_DATA( tpname, type, tpclass, num2dat, dbl2dat ) \
static void								\
store_array_to_##tpname( na_iterator_t *const itr, VALUE arg )		\
{									\
    size_t i, n, s1, s2;						\
    char   *p1, *p2;							\
    size_t *idx1, *idx2;						\
    VALUE x;								\
    double y;								\
    type z;								\
    size_t len, c;							\
    double beg, step;							\
    									\
    INIT_COUNTER( itr, n );						\
    INIT_PTR( itr, 0, p1, s1, idx1 );					\
    INIT_PTR( itr, 1, p2, s2, idx2 );					\
    for (i=0; i<n; i++) {						\
	LOAD_DATA_STEP( p1, s1, idx1, VALUE, x );			\
	if (rb_obj_is_kind_of(x, rb_cRange) || rb_obj_is_kind_of(x, num_cStep)) { \
	    num_step_sequence(x,&len,&beg,&step);			\
	    for (c=0; c<len; c++,i++) {					\
		y = beg + step * c;					\
		{dbl2dat;}						\
		STORE_DATA_STEP( p2, s2, idx2, type, z );		\
	    }								\
	}								\
	else								\
	    if (TYPE(x) != T_ARRAY) {					\
		if (x == Qnil) x = INT2FIX(0);				\
		{num2dat;}						\
		STORE_DATA_STEP( p2, s2, idx2, type, z );		\
	    }								\
    }									\
}									\
static VALUE								\
num_##tpname##_store_array( VALUE nary, VALUE rary )			\
{									\
    VALUE v;								\
    ndfunc_t *func;							\
									\
    func = ndfunc_alloc( store_array_to_##tpname, HAS_LOOP,		\
			 2, 0, Qnil, rb_cArray );			\
    num_##tpname##_fill( nary, INT2FIX(0) );				\
    ndloop_execute_store_array( func, rary, nary );			\
    ndfunc_free(func);							\
    return nary;							\
}									\
static VALUE								\
num_cast_array_to_##tpname( VALUE rary )				\
{									\
    int nd;								\
    size_t *shape;							\
    VALUE tp, nary;							\
    ndfunc_t *func;							\
									\
    shape = na_mdarray_investigate(rary, &nd, &tp);			\
    nary = rb_narray_new(tpclass, nd, shape);				\
    xfree(shape);							\
    num_##tpname##_store_array(nary, rary);				\
    return nary;							\
}

//static VALUE								\
//num_cast_array_to_##tpname( VALUE ary )				\
//{									\
//    VALUE v;								\
//    ndfunc_t *func;							\
//									\
//    func = ndfunc_alloc( cast_array_to_##tpname, HAS_LOOP,		\
//			 1, 1, rb_cArray, tpclass );			\
//    v = ndfunc_execute_from_rarray( func, ary );			\
//    ndfunc_free(func);							\
//    return v;								\
//}


#define BINARY_FUNC( tpname, tpclass, type, opname, operation )		\
static void								\
tpname##_##opname##_loop( na_iterator_t *const itr, VALUE info )	\
{									\
    size_t  i;								\
    char   *p1, *p2, *p3;						\
    ssize_t s1, s2, s3;							\
    ssize_t *idx1, *idx2, *idx3;					\
    type    x, y;							\
    INIT_COUNTER( itr, i );						\
    INIT_PTR( itr, 0, p1, s1, idx1 );					\
    INIT_PTR( itr, 1, p2, s2, idx2 );					\
    INIT_PTR( itr, 2, p3, s3, idx3 );					\
    if (idx1||idx2||idx3) {						\
	for (; i--; ) {							\
	    LOAD_DATA_STEP( p1, s1, idx1, type, x );			\
	    LOAD_DATA_STEP( p2, s2, idx2, type, y );			\
	    operation;							\
	    STORE_DATA_STEP( p3, s3, idx3, type, x );			\
	}								\
    } else {								\
	for (; i--; ) {							\
	    x = *(type*)p1;						\
	    p1+=s1;							\
	    y = *(type*)p2;						\
	    p2+=s2;							\
	    operation;							\
	    *(type*)p3 = x;						\
	    p3+=s3;							\
	}								\
    }									\
}									\
static VALUE								\
num_##tpname##_s_##opname( VALUE mod, VALUE a1, VALUE a2 )		\
{									\
    ndfunc_t *func;							\
    VALUE v;								\
    func = ndfunc_alloc( tpname##_##opname##_loop, HAS_LOOP,		\
			 2, 1, tpclass, tpclass, tpclass );		\
    v = ndfunc_execute( func, 2, a1, a2 );				\
    ndfunc_free(func);							\
    return v;								\
}


#define BINARY_DI_FUNC( tpname, tpclass, type, opname, operation )	\
static void								\
tpname##_##opname##_loop( na_iterator_t *const itr, VALUE info )	\
{									\
    size_t  i;								\
    char   *p1, *p2, *p3;						\
    ssize_t s1, s2, s3;							\
    size_t  e2;								\
    ssize_t *idx1, *idx2, *idx3;					\
    type    x;								\
    int32_t y;								\
    INIT_COUNTER( itr, i );						\
    INIT_PTR( itr, 0, p1, s1, idx1 );					\
    INIT_PTR_ELM( itr, 1, p2, s2, idx2, e2 );				\
    INIT_PTR( itr, 2, p3, s3, idx3 );					\
    if (idx1||idx2||idx3) {						\
	for (; i--; ) {							\
	    LOAD_DATA_STEP( p1, s1, idx1, type, x );			\
	    LOAD_INT_STEP( p2, s2, idx2, e2, int32_t, y );		\
	    operation;							\
	    STORE_DATA_STEP( p3, s3, idx3, type, x );			\
	}								\
    } else {								\
	for (; i--; ) {							\
	    x = *(type*)p1;						\
	    p1+=s1;							\
	    LOAD_INT(p2,e2,y);						\
	    p2+=s2;							\
	    operation;							\
	    *(type*)p3 = x;						\
	    p3+=s3;							\
	}								\
    }									\
}									\
static VALUE								\
num_##tpname##_s_##opname( VALUE mod, VALUE a1, VALUE a2 )		\
{									\
    ndfunc_t *func;							\
    VALUE v;								\
    func = ndfunc_alloc( tpname##_##opname##_loop, HAS_LOOP,		\
			 2, 1, tpclass, cInt32, tpclass );		\
    v = ndfunc_execute( func, 2, a1, a2 );				\
    ndfunc_free(func);							\
    return v;								\
}


#define UNARY_FUNC( tpname, tpclass, type, opname, operation )		\
static void								\
tpname##_##opname##_loop( na_iterator_t *const itr, VALUE info )	\
{									\
    size_t  i;								\
    char   *p1, *p2;							\
    ssize_t s1, s2;							\
    ssize_t *idx1, *idx2;						\
    type    x;								\
    INIT_COUNTER( itr, i );						\
    INIT_PTR( itr, 0, p1, s1, idx1 );					\
    INIT_PTR( itr, 1, p2, s2, idx2 );					\
    if (idx1||idx2) {							\
	for (; i--; ) {							\
	    LOAD_DATA_STEP( p1, s1, idx1, type, x );			\
	    operation;							\
	    STORE_DATA_STEP( p2, s2, idx2, type, x );			\
	}								\
    } else {								\
	for (; i--; ) {							\
	    x = *(type*)p1;						\
	    p1+=s1;							\
	    operation;							\
	    *(type*)p2 = x;						\
	    p2+=s2;							\
	}								\
    }									\
}									\
static VALUE								\
 num_##tpname##_##opname( VALUE a1 )					\
{									\
    ndfunc_t *func;							\
    VALUE v;								\
    func = ndfunc_alloc( tpname##_##opname##_loop, HAS_LOOP,		\
			 1, 1, tpclass, tpclass );			\
    v = ndfunc_execute( func, 1, a1 );					\
    ndfunc_free(func);							\
    return v;								\
}


#define UNARY_FUNC2( tpname, tpclass, type, tpclass2, type2, opname, operation )\
static void								\
tpname##_##opname##_loop( na_iterator_t *const itr, VALUE info )	\
{									\
    size_t  i;								\
    char   *p1, *p2;							\
    ssize_t s1, s2;							\
    ssize_t *idx1, *idx2;						\
    type    x;								\
    type2   y;								\
    INIT_COUNTER( itr, i );						\
    INIT_PTR( itr, 0, p1, s1, idx1 );					\
    INIT_PTR( itr, 1, p2, s2, idx2 );					\
    if (idx1||idx2) {							\
	for (; i--; ) {							\
	    LOAD_DATA_STEP( p1, s1, idx1, type, x );			\
	    operation;							\
	    STORE_DATA_STEP( p2, s2, idx2, type2, y );			\
	}								\
    } else {								\
	for (; i--; ) {							\
	    x = *(type*)p1;						\
	    p1+=s1;							\
	    operation;							\
	    *(type2*)p2 = y;						\
	    p2+=s2;							\
	}								\
    }									\
}									\
static VALUE								\
 num_##tpname##_##opname( VALUE a1 )					\
{									\
    ndfunc_t *func;							\
    VALUE v;								\
    func = ndfunc_alloc( tpname##_##opname##_loop, HAS_LOOP,		\
			 1, 1, tpclass, tpclass2 );			\
    v = ndfunc_execute( func, 1, a1 );					\
    ndfunc_free(func);							\
    return v;								\
}


#define SET_FUNC2( tpname, tpclass, type, tpclass2, type2, opname, operation )\
static void								\
tpname##_##opname##_loop( na_iterator_t *const itr, VALUE info )	\
{									\
    size_t  i;								\
    char   *p1, *p2;							\
    ssize_t s1, s2;							\
    ssize_t *idx1, *idx2;						\
    type    x, *q1;							\
    type2   y;								\
    INIT_COUNTER( itr, i );						\
    INIT_PTR( itr, 0, p1, s1, idx1 );					\
    INIT_PTR( itr, 1, p2, s2, idx2 );					\
    if (idx1||idx2) {							\
	for (; i--; ) {							\
	    LOAD_DATA_TEMP( q1, p1, s1, idx1, type, x );		\
	    LOAD_DATA_STEP( p2, s2, idx2, type2, y );			\
	    operation;							\
	    STORE_DATA( q1, type, x );					\
	}								\
    } else {								\
	for (; i--; ) {							\
	    x = *(type*)p1;						\
	    y = *(type2*)p2;						\
	    p2+=s2;							\
	    operation;							\
	    *(type*)p1 = x;						\
	    p1+=s1;							\
	}								\
    }									\
}									\
static VALUE								\
 num_##tpname##_##opname( VALUE a1, VALUE a2 )				\
{									\
    ndfunc_t *func;							\
    func = ndfunc_alloc( tpname##_##opname##_loop, HAS_LOOP,		\
			 2, 0, Qnil, tpclass2 );			\
    ndfunc_execute( func, 2, a1, a2 );					\
    ndfunc_free(func);							\
    return a2;								\
}


#define UNARY_S_FUNC( tpname, tpclass, type, opname, operation )	\
static void								\
tpname##_##opname##_loop( na_iterator_t *const itr, VALUE info )	\
{									\
    size_t  i;								\
    char   *p1, *p2;							\
    ssize_t s1, s2;							\
    ssize_t *idx1, *idx2;						\
    type    x;								\
    INIT_COUNTER( itr, i );						\
    INIT_PTR( itr, 0, p1, s1, idx1 );					\
    INIT_PTR( itr, 1, p2, s2, idx2 );					\
    if (idx1||idx2) {							\
	for (; i--; ) {							\
	    LOAD_DATA_STEP( p1, s1, idx1, type, x );			\
	    operation;							\
	    STORE_DATA_STEP( p2, s2, idx2, type, x );			\
	}								\
    } else {								\
	for (; i--; ) {							\
	    x = *(type*)p1;						\
	    p1+=s1;							\
	    operation;							\
	    *(type*)p2 = x;						\
	    p2+=s2;							\
	}								\
    }									\
}									\
static VALUE								\
 num_##tpname##_s_##opname( VALUE self, VALUE a1 )			\
{									\
    ndfunc_t *func;							\
    VALUE v;								\
    func = ndfunc_alloc( tpname##_##opname##_loop, HAS_LOOP,		\
			 1, 1, tpclass, tpclass );			\
    v = ndfunc_execute( func, 1, a1 );					\
    ndfunc_free(func);							\
    return v;								\
}


#define CMP_BINARY_FUNC( tpname, tpclass, type, opname, operation )	\
static void								\
tpname##_##opname##_loop( na_iterator_t *const itr, VALUE info )	\
{									\
    size_t  i;								\
    char   *p1, *p2;							\
    BIT_DIGIT *a3;							\
    size_t  p3;								\
    ssize_t s1, s2, s3;							\
    ssize_t *idx1, *idx2, *idx3;					\
    type    x, y;							\
    BIT_DIGIT b;							\
    INIT_COUNTER( itr, i );						\
    INIT_PTR( itr, 0, p1, s1, idx1 );					\
    INIT_PTR( itr, 1, p2, s2, idx2 );					\
    INIT_PTR_BIT( itr, 2, a3, p3, s3, idx3 );				\
    if (idx1||idx2||idx3) {						\
	for (; i--; ) {							\
	    LOAD_DATA_STEP( p1, s1, idx1, type, x );			\
	    LOAD_DATA_STEP( p2, s2, idx2, type, y );			\
	    b = (operation) ? 1:0;					\
	    STORE_BIT_STEP( a3, p3, s3, idx3, b );			\
	}								\
    } else {								\
	for (; i--; ) {							\
	    x = *(type*)p1;						\
	    p1+=s1;							\
	    y = *(type*)p2;						\
	    p2+=s2;							\
	    b = (operation) ? 1:0;					\
	    STORE_BIT(a3,p3,b)						\
	    p3+=s3;							\
	}								\
    }									\
}									\
static VALUE								\
num_##tpname##_s_##opname( VALUE mod, VALUE a1, VALUE a2 )		\
{									\
    ndfunc_t *func;							\
    VALUE v;								\
    func = ndfunc_alloc( tpname##_##opname##_loop, HAS_LOOP,		\
			 2, 1, tpclass, tpclass, cBit );		\
    v = ndfunc_execute( func, 2, a1, a2 );				\
    ndfunc_free(func);							\
    return v;								\
}


#define CMP_UNARY_FUNC( tpname, tpclass, type, opname, operation ) \
static void							  \
tpname##_##opname##_loop( na_iterator_t *const itr, VALUE info ) \
{								\
    size_t   i;							\
    char    *p1;						\
    BIT_DIGIT *a2;						\
    size_t   p2;						\
    ssize_t  s1, s2;						\
    ssize_t *idx1, *idx2;					\
    type    x;							\
    BIT_DIGIT b;						\
    INIT_COUNTER( itr, i );					\
    INIT_PTR( itr, 0, p1, s1, idx1 );				\
    INIT_PTR_BIT( itr, 1, a2, p2, s2, idx2 );			\
    if (idx1||idx2) {						\
	for (; i--; ) {						\
	    LOAD_DATA_STEP( p1, s1, idx1, type, x );		\
	    b = (operation) ? 1:0;				\
	    STORE_BIT_STEP( a2, p2, s2, idx2, b );		\
	}							\
    } else {							\
	for (; i--; ) {						\
	    x = *(type*)p1;					\
	    p1+=s1;						\
	    b = (operation) ? 1:0;				\
	    STORE_BIT(a2,p2,b)					\
	    p2+=s2;						\
	}							\
    }								\
}								\
static VALUE							\
 num_##tpname##_##opname( VALUE a1 )				\
{								\
    ndfunc_t *func;						\
    VALUE v;							\
    func = ndfunc_alloc( tpname##_##opname##_loop, HAS_LOOP,	\
    			1, 1, tpclass, cBit );			\
    v = ndfunc_execute( func, 1, a1 );				\
    return v;							\
}


#define ACCUM_UNARY_FUNC( tpname, tpclass, type, opname, init, condition, operation )\
static void								\
 tpname##_##opname##_loop( na_iterator_t *const itr, VALUE info )	\
{									\
    size_t  i;								\
    char   *p1, *p2;							\
    type   *q2;								\
    ssize_t s1, s2;							\
    ssize_t *idx1, *idx2;						\
    type    x, y;							\
									\
    INIT_COUNTER( itr, i );						\
    INIT_PTR( itr, 0, p1, s1, idx1 );					\
    INIT_PTR( itr, 1, p2, s2, idx2 );					\
    if (idx1||idx2) {							\
	for (; i--; ) {							\
	    LOAD_DATA_STEP( p1, s1, idx1, type, x );			\
	    LOAD_DATA_TEMP( q2, p2, s2, idx2, type, y );		\
	    if (condition) {						\
		{operation;}						\
		STORE_DATA( q2, type, x );				\
            }								\
	}								\
    } else {								\
	for (; i--; ) {							\
	    x = *(type*)p1;						\
	    p1+=s1;							\
	    y = *(type*)p2;						\
	    if (condition) {						\
		{operation;}						\
		STORE_DATA( p2, type, x );				\
	    }								\
	    p2+=s2;							\
	}								\
    }									\
}									\
static VALUE								\
 num_##tpname##_##opname( int argc, VALUE *argv, VALUE self )		\
{									\
    VALUE v, mark;							\
    ndfunc_t *func;							\
    func = ndfunc_alloc( tpname##_##opname##_loop, HAS_LOOP,		\
			 1, 1, tpclass, tpclass );			\
    {init;};								\
    mark = na_mark_dimension( argc, argv, self );			\
    v = ndfunc_execute_reduce( func, mark, 1, self );			\
    ndfunc_free(func);							\
    return num_##tpname##_extract(v);					\
}


#define INSPECT_FUNC(tpname)						\
static VALUE								\
 tpname##_inspect_element( char *ptr, size_t pos, VALUE fmt )		\
{									\
    return format_##tpname( fmt, ptr+pos );				\
}									\
 VALUE									\
 num_##tpname##_inspect( VALUE ary )					\
{									\
    VALUE str = na_info_str( ary );					\
     ndfunc_execute_inspect( ary, str, tpname##_inspect_element, Qnil );\
     return str;							\
}


#define FORMAT_FUNC(tpname)					\
static void							\
tpname##_format_loop( na_iterator_t *const itr, VALUE fmt )	\
{								\
    size_t  i;							\
    char   *p1, *p2;						\
    ssize_t s1, s2;						\
    ssize_t *idx1, *idx2;					\
    void *x;							\
    VALUE y;							\
								\
    INIT_COUNTER( itr, i );					\
    INIT_PTR( itr, 0, p1, s1, idx1 );				\
    INIT_PTR( itr, 1, p2, s2, idx2 );				\
    for (; i--; ) {						\
	LOAD_PTR_STEP( p1, s1, idx1, void, x );			\
	y = format_##tpname( fmt, x );				\
	STORE_DATA_STEP( p2, s2, idx2, VALUE, y );		\
    }								\
}								\
static VALUE							\
 num_##tpname##_format( int argc, VALUE *argv, VALUE self )	\
{								\
     ndfunc_t *func;						\
     VALUE v, fmt=Qnil;						\
								\
     rb_scan_args(argc, argv, "01", &fmt);			\
     func = ndfunc_alloc( tpname##_format_loop, HAS_LOOP,	\
			  1, 1, Qnil, cRObject );		\
     v = ndfunc_execute( func, 2, self, fmt );			\
     ndfunc_free(func);						\
     return v;							\
}								\
static VALUE							\
num_##tpname##_format_to_a( int argc, VALUE *argv, VALUE self )	\
{								\
     ndfunc_t *func;						\
     VALUE v, fmt=Qnil;						\
								\
     rb_scan_args(argc, argv, "01", &fmt);			\
     func = ndfunc_alloc( tpname##_format_loop, HAS_LOOP,	\
			  1, 1, Qnil, rb_cArray );		\
     v = ndfunc_execute_to_rarray( func, self, fmt );		\
     ndfunc_free(func);						\
     return v;							\
}


#define SEQ_DATA_FUNC( tpname, tpclass, type, operation )	\
static void							\
 tpname##_seq_loop( na_iterator_t *const itr, VALUE info )	\
{								\
    size_t i;							\
    char   *p1;							\
    ssize_t  s1;						\
    ssize_t *idx1;						\
    double  x, beg, step;					\
    type    y;							\
    size_t  c;							\
    seq_opt_t *g;						\
    								\
    INIT_COUNTER( itr, i );					\
    INIT_PTR( itr, 0, p1, s1, idx1 );				\
    Data_Get_Struct(info,seq_opt_t,g);				\
    beg  = g->beg;						\
    step = g->step;						\
    c    = g->count;						\
    if (idx1) {							\
	for (; i--; ) {						\
	    x = beg + step * c++;				\
	    {operation;}					\
	    *(type*)(p1+*idx1) = y;				\
	    idx1++;						\
	}							\
    } else {							\
	for (; i--; ) {						\
	    x = beg + step * c++;				\
	    {operation;}					\
	    *(type*)(p1) = y;					\
	    p1 += s1;						\
	}							\
    }								\
    g->count = c;						\
}								\
static VALUE							\
num_##tpname##_seq( int argc, VALUE *args, VALUE self )	\
{								\
    volatile VALUE opt;						\
    seq_opt_t *g;						\
    VALUE vary, vbeg=Qnil, vstep=Qnil;				\
    ndfunc_t *func;						\
    								\
    func = ndfunc_alloc( tpname##_seq_loop, HAS_LOOP,		\
			 1, 0, Qnil );				\
    g = ALLOC(seq_opt_t);					\
    g->beg = 0;							\
    g->step = 1;						\
    g->count = 0;						\
    rb_scan_args(argc, args, "02", &vbeg, &vstep);		\
    if (vbeg!=Qnil) {g->beg = NUM2DBL(vbeg);}			\
    if (vstep!=Qnil) {g->step = NUM2DBL(vstep);}		\
    opt = Data_Wrap_Struct(rb_cData,0,0,g);			\
    ndfunc_execute( func, 2, self, opt );			\
    ndfunc_free(func);						\
    return self;						\
}


#define RAND_DATA_FUNC( tpname, tpclass, type, opname, operation )\
static void							\
 tpname##_##opname##_loop( na_iterator_t *const itr, VALUE info )\
{								\
    size_t   i;							\
    char    *p1;						\
    ssize_t  s1;						\
    ssize_t *idx1;						\
    type     y;							\
    								\
    INIT_COUNTER( itr, i );					\
    INIT_PTR( itr, 0, p1, s1, idx1 );				\
    if (idx1) {							\
	for (; i--; ) {						\
	    {operation};					\
	    *(type*)(p1+*idx1) = y;				\
	    idx1++;						\
	}							\
    } else {							\
	for (; i--; ) {						\
	    {operation};					\
	    *(type*)(p1) = y;					\
	    p1+=s1;						\
	}							\
    }								\
}								\
static VALUE							\
 num_##tpname##_##opname( VALUE self )				\
{								\
    ndfunc_t *func;						\
    func = ndfunc_alloc( tpname##_##opname##_loop, HAS_LOOP,	\
			 1, 0, Qnil );				\
    ndfunc_execute( func, 1, self );				\
    ndfunc_free(func);						\
    return self;						\
}


#define RAND_NORM_DATA_FUNC( tpname, tpclass, type, operation )	\
static void							\
 tpname##_rand_norm_loop( na_iterator_t *const itr, VALUE info )\
{								\
    size_t   i;							\
    char    *p1;						\
    ssize_t  s1;						\
    ssize_t *idx1;						\
    type     y;							\
    type     a[2];						\
    								\
    INIT_COUNTER( itr, i );					\
    INIT_PTR( itr, 0, p1, s1, idx1 );				\
    if (idx1) {							\
	for (; i>1; i-=2) {					\
	    {operation;}					\
	    *(type*)(p1+*idx1)     = a[0];			\
	    *(type*)(p1+*(idx1+1)) = a[1];			\
	    idx1 += 2;						\
	}							\
	if (i>0) {						\
	    {operation};					\
	    *(type*)(p1+*idx1) = a[0];				\
	}							\
    } else {							\
	for (; i>1; i-=2) {					\
	    {operation;}					\
	    *(type*)(p1)    = a[0];				\
	    *(type*)(p1+s1) = a[1];				\
	    p1 += s1*2;						\
	}							\
	if (i>0) {						\
	    {operation;}					\
	    *(type*)(p1) = a[0];				\
	}							\
    }								\
}								\
static VALUE							\
 num_##tpname##_rand_norm( VALUE self )				\
{								\
    ndfunc_t *func;						\
    func = ndfunc_alloc( tpname##_rand_norm_loop, HAS_LOOP,	\
			 1, 0, Qnil );				\
    ndfunc_execute( func, 1, self );				\
    ndfunc_free(func);						\
    return self;						\
}


#define FILL_DATA_WITH_NUM( tpname, type, tpclass, operation )	\
static void							\
 tpname##_fill_loop( na_iterator_t *const itr, VALUE x )	\
{								\
    size_t   i;							\
    char    *p1;						\
    ssize_t  s1;						\
    ssize_t *idx1;						\
    type     y;							\
    INIT_COUNTER( itr, i );					\
    INIT_PTR( itr, 0, p1, s1, idx1 );				\
    {operation;}						\
    if (idx1) {							\
	for (; i--; ) {*(type*)(p1+*(idx1++)) = y;}		\
    } else {							\
	for (; i--; ) {*(type*)(p1) = y; p1+=s1;}		\
    }								\
}								\
 static VALUE							\
 num_##tpname##_fill( VALUE self, VALUE val )			\
{								\
    ndfunc_t *func;						\
    func = ndfunc_alloc( tpname##_fill_loop, HAS_LOOP,		\
			 1, 0, Qnil );				\
    ndfunc_execute( func, 2, self, val );			\
    ndfunc_free(func);						\
    return self;						\
}


#define POW_DATA_INT(type)			\
static type pow_##type##_int(type x, int p)	\
{						\
    type r=1;					\
    switch(p) {					\
    case 2: return x*x;				\
    case 3: return x*x*x;			\
    case 4: x=x*x; return x*x;			\
    case 1: return x;				\
    case 0: return 1;				\
    }						\
    if (p<0)  return 1/pow_##type##_int(x,-p);	\
    if (p>64) return pow(x,p);			\
    while (p) {					\
	if ( (p%2) == 1 ) r *= x;		\
	x *= x;					\
	p /= 2;					\
    }						\
    return r;					\
}
