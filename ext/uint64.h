typedef int64_t dtype;
typedef int64_t rtype;

#define m_zero 0
#define m_one  1

#define m_num_to_data(x) NUM2INT64(x)
#define m_data_to_num(x) INT642NUM(x)

#define m_from_double(x) (x)
#define m_from_real(x) (x)

#define m_extract(x) INT642NUM(*(dtype*)x)

#define m_add(x,y) ((x)+(y))
#define m_sub(x,y) ((x)-(y))
#define m_mul(x,y) ((x)*(y))
#define m_div(x,y) ((x)/(y))
#define m_mod(x,y) ((x)%(y))
#define m_pow(x,y) pow_int64(x,y)

#define m_bit_and(x,y) ((x)&(y))
#define m_bit_or(x,y)  ((x)|(y))
#define m_bit_xor(x,y) ((x)^(y))
#define m_bit_not(x)   (~(x))

#define m_abs(x)     ((x<0)?-x:x)
#define m_minus(x)   (-(x))
#define m_inverse(x) (1/(x))
#define m_square(x)  ((x)*(x))

#define m_eq(x,y) ((x)==(y))
#define m_ne(x,y) ((x)!=(y))
#define m_gt(x,y) ((x)>(y))
#define m_ge(x,y) ((x)>=(y))
#define m_lt(x,y) ((x)<(y))
#define m_le(x,y) ((x)<=(y))

#define m_sum(x,y) {{y+=x;}}
#define m_sum_init INT2FIX(0)
#define m_min(x,y) {if (!isnan(x) && (isnan(y) || y>x)) {y=x;}}
#define m_min_init nary_init_accum_aref0(self, accum)
#define m_max(x,y) {if (!isnan(x) && (isnan(y) || y<x)) {y=x;}}
#define m_max_init nary_init_accum_aref0(self, accum)

#define m_rand gen_rand32()

#define m_sprintf(s,x) sprintf(s,"%"INT64FMT"d",x)

#define cmp(a,b)                                        \
    ((qsort_cast(a)==qsort_cast(b)) ? 0 :               \
     (qsort_cast(a) > qsort_cast(b)) ? 1 : -1)
#define cmpgt(a,b)                              \
    (qsort_cast(a) > qsort_cast(b))
