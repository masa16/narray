/*
  narray.h
  Numerical Array Extention for Ruby
    (C) Copyright 1999-2003 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/
#ifndef TEMPLATE_H
#define TEMPLATE_H

// typedef unsigned int BIT_DIGIT;
// //#define BYTE_BIT_DIGIT sizeof(BIT_DIGIT)
// #define NB     (sizeof(BIT_DIGIT)*8)
// #define BALL   (~(BIT_DIGIT)0)
// #define SLB(n) (((n)==NB)?~(BIT_DIGIT)0:(~(~(BIT_DIGIT)0<<(n))))

#define RET_FIXNUM(func,num)		\
    static VALUE func(VALUE self)	\
    { return INT2FIX(num); }

#define RET_FLOAT(func,num)		\
    static VALUE func(VALUE self)	\
    { return rb_float_new(num); }

#define INIT_COUNTER( itr, c )		\
    {	c = (itr[0]).n; }

#define INIT_PTR( itr, i, pt, st, id )		\
    {						\
	pt = (itr[i]).ptr + (itr[i]).pos;	\
	st = (itr[i]).step;			\
	id = (itr[i]).idx;			\
    }

#define INIT_PTR_ELM( itr, i, pt, st, id, es )	\
    {						\
	pt = (itr[i]).ptr + (itr[i]).pos;	\
	st = (itr[i]).step;			\
	id = (itr[i]).idx;			\
	es = (itr[i]).elmsz;			\
    }

#define LOAD_PTR_STEP( ptr, step, idx, type, adr ) \
{						\
    if (idx) {					\
	adr = (type*)(ptr + *idx);		\
	idx++;					\
    } else {					\
	adr = (type*)(ptr);			\
	ptr += step;				\
    }						\
}

#define LOAD_DATA_STEP( ptr, step, idx, type, val ) \
{						\
    if (idx) {					\
	val = *(type*)(ptr + *idx);		\
	idx++;					\
    } else {					\
	val = *(type*)(ptr);			\
	ptr += step;				\
    }						\
}

#define LOAD_DATA_TEMP( tmp_p, ptr, step, idx, type, val ) \
{						\
    if (idx) {					\
	tmp_p = (type*)(ptr + *idx);		\
	idx++;					\
    } else {					\
	tmp_p = (type*)(ptr);			\
	ptr += step;				\
    }						\
    val = *tmp_p;				\
}

#define LOAD_DATA_NOSTEP( ptr, ste, idx, type, val )	\
{						\
    if (idx) {					\
	val = *(type*)(ptr + *idx);		\
    } else {					\
	val = *(type*)(ptr);			\
    }						\
}

#define STORE_DATA_STEP( ptr, step, idx, type, val ) \
{						\
    if (idx) {					\
	*(type*)(ptr + *idx) = val;		\
	idx++;					\
    } else {					\
	*(type*)(ptr) = val;			\
	ptr += step;				\
    }						\
}

#define STORE_DATA( ptr, type, val ) \
{			\
  *(type*)ptr = val;	\
}


#define LOAD_INT_STEP( ptr, step, idx, esz, type, val )		\
{								\
    type *p;							\
    if (idx) {							\
	p = (type*)(ptr + *idx);				\
	idx++;							\
    } else {							\
	p = (type*)(ptr);					\
	ptr += step;						\
    }								\
    LOAD_INT(p,esz,val);					\
}

#define LOAD_INT_TEMP( tmp_p, ptr, step, idx, esz, type, val )	\
{								\
    if (idx) {							\
	tmp_p = (type*)(ptr + *idx);				\
	idx++;							\
    } else {							\
	tmp_p = (type*)(ptr);					\
	ptr += step;						\
    }								\
    LOAD_INT(tmp_p,esz,val);					\
}

#define LOAD_INT_NOSTEP( ptr, idx, esz, type, val )	\
{						\
    type *p;					\
    if (idx) {					\
	p = (type*)(ptr + *idx);		\
    } else {					\
	p = (type*)(ptr);			\
    }						\
    LOAD_INT(p,esz,val);			\
}

//#ifdef WORDS_BIGENDIAN
//#define LOAD_INT( ptr, esz, x ) \
//  { x=((char*)(ptr))[0]; memcpy(((char*)&(x))+(sizeof(x)-(esz)),ptr,esz); } // big endian
//#define LOAD_UINT( ptr, esz, x ) \
//  { x=0; memcpy(((char*)&(x))+(sizeof(x)-(esz)),ptr,esz); } // big endian
//#else
//#define LOAD_INT( ptr, esz, x )					\
//  { x=((char*)(ptr))[(esz)-1]; memcpy(&(x),ptr,esz); } // little endian
//#define LOAD_UINT( ptr, esz, x )					\
//  { x=0; memcpy(&(x),ptr,esz); } // little endian
//#endif


#define STORE_INT_STEP( ptr, step, idx, esz, type, val )\
  {							\
      type *p;						\
      if (idx) {					\
	  p = (type*)(ptr + *idx);			\
	  idx++;					\
      } else {						\
	  p = (type*)(ptr);				\
	  ptr += step;					\
      }							\
      STORE_INT(p,esz,val);				\
  }


#define INIT_PTR_BIT( itr, i, ad, ps, st, id )	\
    {						\
        ps = (itr[i]).pos;			\
        ad = (BIT_DIGIT*)((itr[i]).ptr) + ps/NB;\
	ps %= NB;				\
        st = (itr[i]).step;			\
	id = (itr[i]).idx;			\
    }

#define LOAD_BIT( adr, pos, val )			\
    {							\
	size_t dig = (pos) / NB;			\
	int    bit = (pos) % NB;			\
	val = (((BIT_DIGIT*)(adr))[dig]>>(bit)) & 1u;	\
    }

#define LOAD_BIT_STEP( adr, pos, step, idx, val )	\
    {							\
	size_t dig; int bit;				\
        if (idx) {					\
	    dig = ((pos) + *(idx)) / NB;		\
	    bit = ((pos) + *(idx)) % NB;		\
	    idx++;					\
	} else {					\
	    dig = (pos) / NB;				\
	    bit = (pos) % NB;				\
	    pos += step;				\
	}						\
	val = (((BIT_DIGIT*)(adr))[dig]>>bit) & 1u;	\
    }

#define STORE_BIT(adr,pos,val)			\
    {						\
	size_t dig = (pos) / NB;		\
	int    bit = (pos) % NB;		\
	((BIT_DIGIT*)(adr))[dig] =		\
	    ((BIT_DIGIT*)(adr))[dig] &		\
	    ~(1u<<(bit)) | ((val)<<(bit));	\
    }
// val -> val&1 ??

#define STORE_BIT_STEP( adr, pos, step, idx, val )\
    {						\
	size_t dig; int bit;			\
        if (idx) {				\
	    dig = ((pos) + *(idx)) / NB;	\
	    bit = ((pos) + *(idx)) % NB;	\
	    idx++;				\
        } else {				\
	    dig = (pos) / NB;			\
	    bit = (pos) % NB;			\
	    pos += step;			\
	}					\
	((BIT_DIGIT*)(adr))[dig] =		\
	    ((BIT_DIGIT*)(adr))[dig] &		\
	    ~(1u<<(bit)) | ((val)<<(bit));	\
    }
// val -> val&1 ??


#if !defined(LOAD_INT)
#ifdef WORDS_BIGENDIAN
#define LOAD_INT( ptr, esz, x ) \
  { x=((char*)(ptr))[0]; memcpy(((char*)&(x))+(sizeof(x)-(esz)),ptr,esz); } // big endian
#define STORE_INT( ptr, esz, x ) \
  memcpy(ptr,((char*)&(x))+(sizeof(x)-(esz)),esz) // big endian
#else
#define LOAD_INT( ptr, esz, x )					\
  { x=((char*)(ptr))[(esz)-1]; memcpy(&(x),ptr,esz); } // little endian
#define STORE_INT( ptr, esz, x ) \
  memcpy(ptr,&(x),esz) // little endian
#endif
#endif

#endif /* ifndef NARRAY_H */
