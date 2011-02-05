/*
  template_int.c
  Numerical Array Extention for Ruby
    (C) Copyright 1999-2007 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/

#define CAST_INT_TO_NUM( intname, inttype, int2num ) \
static VALUE \
num_##intname##_extract( VALUE self ) \
{ \
    VALUE klass; \
    char *ptr; \
    int esz; \
    inttype x; \
    narray_t *na; \
    GetNArray(self,na); \
    if (na->ndim==0) { \
        ptr = (char*)na_data_pointer_for_read(self) + na->offset; \
	klass = CLASS_OF(self); \
	SET_INT_ELEMENT_SIZE(klass,esz); \
	LOAD_INT( ptr, esz, x ); \
	return int2num(x); \
    } \
    return self; \
}


#define CAST_NUM_TO_INT( intname, inttype, num2int ) \
static VALUE \
num_##intname##_new_dim0( VALUE klass, inttype x ) \
{ \
    narray_t *na; \
    VALUE v; \
    char *ptr; \
    int esz; \
    v = rb_narray_new( klass, 0, NULL ); \
    GetNArray(v,na); \
    ptr = (char*)na_data_pointer_for_write(v) + na->offset; \
    SET_INT_ELEMENT_SIZE(klass,esz); \
    STORE_INT( ptr, esz, x ); \
    return v; \
} \
static VALUE \
num_cast_numeric_to_##intname( VALUE klass, VALUE val ) \
{ \
    return num_##intname##_new_dim0( klass, num2int(val) ); \
}


// DFloat --> Int32..Int8

#define CAST_DATA_TO_INT( dataname, datatype, intname, inttype ) \
static void \
dataname##_to_##intname( na_iterator_t *const itr, VALUE info ) \
{ \
    size_t  i, s1, s2; \
    char   *p1, *p2; \
    size_t *idx1, *idx2; \
    int     e2; \
    datatype x; \
    inttype  y; \
 \
    INIT_COUNTER( itr, i ); \
    INIT_PTR( itr, 0, p1, s1, idx1 ); \
    INIT_PTR_ELM( itr, 1, p2, s2, idx2, e2 ); \
    if (idx1||idx2) \
	for (; i--; ) { \
	    LOAD_DATA_STEP( p1, s2, idx1, datatype, x ); \
	    y = x; \
	    STORE_INT_STEP( p2, s2, idx2, e2, inttype, y ); \
	} \
    else \
	for (; i--; ) { \
	    x = *(datatype*)p1; \
	    p1+=s1; \
	    y = x; \
	    STORE_INT(p2,e2,y); \
	    p2+=s2; \
	} \
} \
static VALUE \
num_cast_##dataname##_to_##intname( VALUE klass, VALUE self ) \
{ \
    VALUE v; \
    ndfunc_t *func; \
    func = ndfunc_alloc( dataname##_to_##intname, HAS_LOOP, 1, 1, Qnil, klass ); \
    v = ndfunc_execute( func, 1, self ); \
    ndfunc_free( func ); \
    return v; \
}


// Int -> DFloat
#define CAST_INT_TO_DATA( intname, inttype, dataname, datatype, dataclass, operation ) \
static void \
intname##_cast_to_##dataname( na_iterator_t *const itr, VALUE info ) \
{ \
    size_t  i, s1, s2; \
    char   *p1, *p2; \
    size_t *idx1, *idx2; \
    int     e1; \
    inttype x; \
    datatype  y; \
    INIT_COUNTER( itr, i ); \
    INIT_PTR_ELM( itr, 0, p1, s1, idx1, e1 ); \
    INIT_PTR( itr, 1, p2, s2, idx2 ); \
    for (; i--; ) { \
	LOAD_INT_STEP( p1, s1, idx1, e1, inttype, x ); \
        operation; \
	STORE_DATA_STEP( p2, s2, idx2, datatype, y ); \
    } \
} \
VALUE \
num_cast_##intname##_to_##dataname( VALUE self ) \
{ \
    VALUE v; \
    ndfunc_t *func; \
    func = ndfunc_alloc( intname##_cast_to_##dataname, HAS_LOOP, 1, 1, Qnil, dataclass ); \
    v = ndfunc_execute( func, 1, self ); \
    ndfunc_free(func); \
    return v; \
}


#define CAST_INT_TO_ARRAY( tpname, type, int2num )		\
void								\
 tpname##_cast_to_array( na_iterator_t *const itr, VALUE info ) \
{								\
    size_t i, s1, s2;						\
    char   *p1, *p2;						\
    size_t *idx1, *idx2;					\
    int     e1;							\
    type    x;							\
    volatile VALUE y;						\
    INIT_COUNTER( itr, i );					\
    INIT_PTR_ELM( itr, 0, p1, s1, idx1, e1 );			\
    INIT_PTR( itr, 1, p2, s2, idx2 );				\
    for (; i--; ) {						\
	LOAD_INT_STEP( p1, s1, idx1, e1, type, x );		\
        y = int2num(x);						\
	STORE_DATA_STEP( p2, s2, idx2, VALUE, y );		\
    }								\
}								\
 static VALUE							\
 num_cast_##tpname##_to_array( VALUE self )			\
{								\
    VALUE v;							\
    ndfunc_t *func;						\
    func = ndfunc_alloc( tpname##_cast_to_array, HAS_LOOP,	\
			 1, 1, Qnil, rb_cArray );		\
    v = ndfunc_execute_to_rarray( func, self, Qnil );		\
    ndfunc_free(func);						\
    return v;							\
}


#define CAST_ARRAY_TO_INT( tpname, type, numtoint ) \
static void \
tpname##_store_array_loop( na_iterator_t *const itr, VALUE arg ) \
{ \
    size_t i, n, s1, s2; \
    char   *p1, *p2; \
    size_t *idx1, *idx2; \
    int    e2; \
    VALUE   x; \
    type    y; \
    size_t len, c; \
    double beg, step; \
 \
    INIT_COUNTER( itr, n ); \
    INIT_PTR( itr, 0, p1, s1, idx1 ); \
    INIT_PTR_ELM( itr, 1, p2, s2, idx2, e2 ); \
    for (i=0; i<n; i++) { \
	LOAD_DATA_STEP( p1, s1, idx1, VALUE, x ); \
        if (rb_obj_is_kind_of(x, rb_cRange) || rb_obj_is_kind_of(x, num_cStep)) { \
	    num_step_sequence(x,&len,&beg,&step); \
	    for (c=0; c<len; c++,i++) { \
		y = beg + step * c; \
		STORE_INT_STEP( p2, s2, idx2, e2, type, y ); \
	    } \
	} \
	else \
	if (TYPE(x) != T_ARRAY) { \
	    if (x != Qnil) { \
		y = numtoint(x); \
	    } else { \
		y = 0; \
	    } \
	    STORE_INT_STEP( p2, s2, idx2, e2, type, y ); \
	} \
    } \
} \
static VALUE								\
num_##tpname##_store_array( VALUE nary, VALUE rary )			\
{									\
    VALUE v;								\
    ndfunc_t *func;							\
									\
    func = ndfunc_alloc( tpname##_store_array_loop, HAS_LOOP,		\
			 2, 0, Qnil, rb_cArray );			\
    num_##tpname##_fill( nary, INT2FIX(0) );				\
    ndloop_execute_store_array( func, rary, nary );			\
    ndfunc_free(func);							\
    return nary;							\
}									\
static VALUE								\
num_cast_array_to_##tpname( VALUE klass, VALUE rary )			\
{									\
    int nd;								\
    size_t *shape;							\
    VALUE tp, nary;							\
    ndfunc_t *func;							\
									\
    shape = na_mdarray_investigate(rary, &nd, &tp);			\
    nary = rb_narray_new(klass, nd, shape);				\
    xfree(shape);							\
    num_##tpname##_store_array(nary, rary);				\
    return nary;							\
}

//static VALUE \
//num_cast_array_to_##tpname( VALUE klass, VALUE ary ) \
//{ \
//    VALUE v; \
//    ndfunc_t *func; \
//    func = ndfunc_alloc( tpname##_cast_from_array, HAS_LOOP, 1, 1, rb_cArray, klass ); \
//    num_##dataname##_fill( nary, INT2FIX(0) );				\
//    ndloop_execute_store_array( func, rary, nary );			\
//    v = ndloop_execute_from_rarray( func, ary ); \
//    ndfunc_free( func ); \
//    return v; \
//}


#define DEBUG_PRINT_INT(intname,esz,type,fmt)	\
static void					\
 intname##_print_loop( char *ptr, size_t pos, VALUE opt ) \
{						\
    type x;					\
    LOAD_INT( ptr+pos, esz, x );		\
    printf(fmt, x);				\
}						\
static VALUE					\
 num_##intname##_debug_print( VALUE ary )	\
{						\
    ndfunc_debug_print( ary, intname##_print_loop, Qnil );	\
    return Qnil;				\
}


#define BINARY_INT_FUNC( tpname, tpclass, type, opname, operation )		\
static void								\
tpname##_##opname##_loop( na_iterator_t *const itr, VALUE info )	\
{									\
    size_t  i;								\
    char   *p1, *p2, *p3;						\
    ssize_t s1, s2, s3;							\
    ssize_t *idx1, *idx2, *idx3;					\
    int     e1, e2, e3;							\
    type    x, y;							\
    INIT_COUNTER( itr, i );						\
    INIT_PTR_ELM( itr, 0, p1, s1, idx1, e1 );				\
    INIT_PTR_ELM( itr, 1, p2, s2, idx2, e2 );				\
    INIT_PTR_ELM( itr, 2, p3, s3, idx3, e3 );				\
    if (idx1||idx2||idx3||						\
	e1!=sizeof(type)||e2!=sizeof(type)||e3!=sizeof(type)) {		\
	for (; i--; ) {							\
	    LOAD_INT_STEP( p1, s1, idx1, e1, type, x );			\
	    LOAD_INT_STEP( p2, s2, idx2, e2, type, y );			\
	    operation;							\
	    STORE_INT_STEP( p3, s3, idx3, e3, type, x );		\
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
    VALUE v, c1, c2;							\
    ndfunc_t *func;							\
    if (rb_obj_is_kind_of(a1,mod))					\
        c1 = CLASS_OF(a1); \
    else \
	c1 = mod; \
    if (rb_obj_is_kind_of(a2,mod))					\
        c2 = CLASS_OF(a2); \
    else \
	c2 = mod; \
    func = ndfunc_alloc( tpname##_##opname##_loop, HAS_LOOP,		\
			    2, 1, c1, c2, mod );			\
    v = ndfunc_execute( func, 2, a1, a2 );				\
    ndfunc_free( func );						\
    return v;								\
}


#define UNARY_INT_FUNC( tpname, tpclass, type, opname, operation )	\
static void								\
tpname##_##opname##_loop( na_iterator_t *const itr, VALUE info )	\
{									\
    size_t  i;								\
    char   *p1, *p2;							\
    ssize_t s1, s2;							\
    ssize_t *idx1, *idx2;						\
    int     e1, e2;							\
    type    x;								\
    INIT_COUNTER( itr, i );						\
    INIT_PTR_ELM( itr, 0, p1, s1, idx1, e1 );				\
    INIT_PTR_ELM( itr, 1, p2, s2, idx2, e2 );				\
    if (idx1||idx2||							\
	e1!=sizeof(type)||e2!=sizeof(type)) {				\
	for (; i--; ) {							\
	    LOAD_INT_STEP( p1, s1, idx1, e1, type, x );			\
	    operation;							\
	    STORE_INT_STEP( p2, s2, idx2, e2, type, x );		\
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
    VALUE v, klass;							\
    ndfunc_t *func;							\
    klass = CLASS_OF(a1);						\
    func = ndfunc_alloc( tpname##_##opname##_loop, HAS_LOOP,		\
			    1, 1, klass, klass );			\
    v = ndfunc_execute( func, 1, a1 );					\
    ndfunc_free( func );						\
    return v;								\
}


#define CMP_BINARY_INT_FUNC( tpname, tpclass, type, opname, operation )	\
static void								\
tpname##_##opname##_loop( na_iterator_t *const itr, VALUE info )	\
{									\
    size_t  i;								\
    char   *p1, *p2;							\
    BIT_DIGIT *a3;							\
    size_t  p3;								\
    ssize_t s1, s2, s3;							\
    ssize_t *idx1, *idx2, *idx3;					\
    int     e1, e2;							\
    type    x, y;							\
    BIT_DIGIT b;							\
    INIT_COUNTER( itr, i );						\
    INIT_PTR_ELM( itr, 0, p1, s1, idx1, e1 );				\
    INIT_PTR_ELM( itr, 1, p2, s2, idx2, e2 );				\
    INIT_PTR_BIT( itr, 2, a3, p3, s3, idx3 );				\
    if (idx1||idx2||idx3||						\
	e1!=sizeof(type)||e2!=sizeof(type)) {				\
	for (; i--; ) {							\
	    LOAD_INT_STEP( p1, s1, idx1, e1, type, x );			\
	    LOAD_INT_STEP( p2, s2, idx2, e2, type, y );			\
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
    VALUE v;								\
    ndfunc_t *func;							\
    if (!rb_obj_is_kind_of(a1,tpclass))					\
	rb_raise(rb_eTypeError,"wrong class: %s, expected: %s",		\
		 rb_obj_classname(a1),rb_class2name(tpclass));		\
    if (!rb_obj_is_kind_of(a2,tpclass))					\
	rb_raise(rb_eTypeError,"wrong class: %s, expected: %s",		\
		 rb_obj_classname(a2),rb_class2name(tpclass));		\
    func = ndfunc_alloc( tpname##_##opname##_loop, HAS_LOOP,		\
			    2, 1, CLASS_OF(a1), CLASS_OF(a2), cBit );	\
    v = ndfunc_execute( func, 2, a1, a2 );				\
    ndfunc_free( func );						\
    return v;								\
}


#define ACCUM_UNARY_INT_FUNC( tpname, tpclass, type, opname, init, condition, operation ) \
static void								\
 tpname##_##opname##_loop( na_iterator_t *const itr, VALUE info )	\
{									\
    size_t  i;								\
    char   *p1, *p2;							\
    type   *q2;								\
    ssize_t s1, s2;							\
    ssize_t *idx1, *idx2;						\
    int     e1, e2;							\
    type    x, y;							\
									\
    INIT_COUNTER( itr, i );						\
    INIT_PTR_ELM( itr, 0, p1, s1, idx1, e1 );				\
    INIT_PTR_ELM( itr, 1, p2, s2, idx2, e2 );				\
    if (idx1||idx2||							\
	e1!=sizeof(type)||e2!=sizeof(type)) {				\
	for (; i--; ) {							\
	    LOAD_INT_STEP( p1, s1, idx1, e1, type, x );			\
	    LOAD_INT_TEMP( q2, p2, s2, idx2, e2, type, y );		\
	    if (condition) {						\
	        operation;						\
	        STORE_INT( q2, e2, x );					\
	    }								\
	}								\
    } else {								\
	for (; i--; ) {							\
	    x = *(type*)p1;						\
	    p1+=s1;							\
	    y = *(type*)p2;						\
	    if (condition) {						\
	        {operation;}						\
	    	*(type*)p2 = x;						\
	    }								\
	    p2+=s2;							\
	}								\
    }									\
}									\
static VALUE								\
 num_##tpname##_##opname( int argc, VALUE *argv, VALUE self )		\
{									\
    VALUE v, mark, klass;						\
    ndfunc_t *func;							\
    mark = na_mark_dimension( argc, argv, self );			\
    klass = CLASS_OF(self);						\
    func = ndfunc_alloc( tpname##_##opname##_loop, HAS_LOOP,		\
			    1, 1, klass, klass );			\
    {init;};								\
    v = ndfunc_execute_reduce( func, mark, 1, self );			\
    v = num_##tpname##_extract(v);					\
    return v;								\
}


#define INT_SEQ( intname, inttype )				\
static void							\
 intname##_seq( na_iterator_t *const itr, VALUE info )		\
{								\
    size_t i;							\
    char   *p1;							\
    ssize_t  s1;						\
    ssize_t *idx1;						\
    size_t   e1;						\
    inttype  x;							\
    double  beg, step;						\
    size_t  c;							\
    seq_opt_t *g;						\
								\
    INIT_COUNTER( itr, i );					\
    INIT_PTR_ELM( itr, 0, p1, s1, idx1, e1 );			\
    Data_Get_Struct(info,seq_opt_t,g);				\
    beg  = g->beg;						\
    step = g->step;						\
    c    = g->count;						\
    if (idx1) {							\
	for (; i--; ) {						\
	    x = beg + step * c++;				\
	    STORE_INT(p1+*idx1,e1,x);				\
	    idx1++;						\
	}							\
    } else {							\
	for (; i--; ) {						\
	    x = beg + step * c++;				\
	    STORE_INT(p1,e1,x);					\
	    p1 += s1;						\
	}							\
    }								\
    g->count = c;						\
}								\
static VALUE							\
 num_##intname##_seq( int argc, VALUE *args, VALUE self )	\
{								\
    volatile VALUE opt;						\
    seq_opt_t *g;						\
    VALUE vary, vbeg=Qnil, vstep=Qnil;				\
    ndfunc_t *func;						\
								\
    func = ndfunc_alloc( intname##_seq, HAS_LOOP, 1, 0, Qnil ); \
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


#define FILL_INT_WITH_NUM( tpname, type, operation )		\
static void							\
 tpname##_fill_loop( na_iterator_t *const itr, VALUE x )	\
{								\
    size_t   i;							\
    char    *p1;						\
    size_t   e1;						\
    ssize_t  s1;						\
    ssize_t *idx1;						\
    type     y;							\
    INIT_COUNTER( itr, i );					\
    INIT_PTR_ELM( itr, 0, p1, s1, idx1, e1 );			\
    {operation;}						\
    if (idx1) {							\
	for (; i--; ) {						\
	    STORE_INT(p1+*idx1,e1,y);				\
	    idx1++;						\
	}							\
    } else {							\
	for (; i--; ) {						\
	    STORE_INT(p1,e1,y);					\
	    p1 += s1;						\
	}							\
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


#define RAND_INT_FUNC(tpname,type,genrand,genbits,issigned)	\
static void							\
 tpname##_rand_loop( na_iterator_t *const itr, VALUE opt )\
{							\
    size_t   i;						\
    char    *p1;					\
    size_t   e1;					\
    ssize_t  s1;					\
    ssize_t *idx1;					\
    type     max, x;					\
    int      shift;					\
    rand_opt_t *g;					\
    Data_Get_Struct(opt,rand_opt_t,g);			\
    max   = g->max;					\
    shift = g->shift;					\
    INIT_COUNTER( itr, i );				\
    INIT_PTR_ELM( itr, 0, p1, s1, idx1, e1 );		\
    if (idx1) {						\
	for (; i--; ) {					\
	    do {x=genrand()>>shift;}			\
	    while (x>max);				\
	    STORE_INT(p1+*idx1,e1,x);			\
	    idx1++;					\
	}						\
    } else {						\
	for (; i--; ) {					\
	    do {x=genrand()>>shift;}			\
	    while (x>max);				\
	    STORE_INT(p1,e1,x);				\
	    p1 += s1;					\
	}						\
    }							\
}									\
static VALUE								\
num_##tpname##_rand( int argc, VALUE *args, VALUE self )		\
{									\
    VALUE opt, vmax=Qnil;						\
    ndfunc_t *func;							\
    rand_opt_t *g;							\
    u_int64_t max;							\
    int bitsize;							\
									\
    bitsize = FIX2INT(rb_const_get(CLASS_OF(self),id_element_bit_size));\
    bitsize -= issigned;						\
									\
    g = ALLOC(rand_opt_t);						\
    g->max   = ((u_int64_t)1<<(bitsize))-1;				\
    g->shift = (genbits)-(bitsize);					\
    g->sign  = 1;							\
									\
    rb_scan_args(argc, args, "01", &vmax);				\
    if (!NIL_P(vmax)) {							\
	if (RTEST(rb_funcall(vmax,'<',1,INT2FIX(1)))) {			\
	    rb_raise(rb_eArgError,"rand-max must be positive");		\
	}								\
	max = NUM2UINT64(vmax);						\
	if (max>g->max) {						\
	    rb_raise(rb_eArgError,"rand-max is too big");		\
	}								\
	g->max = max;							\
	g->shift = (genbits)-n_bits(max);				\
    }									\
    opt = Data_Wrap_Struct(rb_cData,0,0,g);				\
									\
    func = ndfunc_alloc( tpname##_rand_loop, HAS_LOOP, 1, 0, Qnil );	\
    ndfunc_execute( func, 2, self, opt );				\
    ndfunc_free(func);							\
    return self;							\
}


#define FORMAT_INT(tpname,type,elmsz,strsz,fmt)	\
static VALUE					\
format_##tpname( VALUE f, void *x )		\
{						\
    type y;					\
    LOAD_INT(x,elmsz,y);			\
    if (NIL_P(f)) {				\
	char s[strsz];				\
	int n;					\
	n = sprintf(s,fmt,y);			\
	return rb_str_new(s,n);			\
    }						\
    return rb_funcall( f, '%', 1, INT2NUM(y) );	\
}
