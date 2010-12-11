/*
  template_comp.c
  Numerical Array Extention for Ruby
    (C) Copyright 1999-2007 by Masahiro TANAKA

  This program is free software.
  You can distribute/modify this program
  under the same terms as Ruby itself.
  NO WARRANTY.
*/

#ifndef M_PI_2
#define M_PI_2         1.57079632679489661923  /* pi/2 */
#endif
#ifndef M_LOG2E
#define M_LOG2E        1.4426950408889634074   /* log_2 e */
#endif
#ifndef M_LOG10E
#define M_LOG10E       0.43429448190325182765  /* log_10 e */
#endif
#ifndef M_LN2
#define M_LN2          0.69314718055994530942  /* log_e 2 */
#endif
#ifndef M_LN10
#define M_LN10         2.30258509299404568402  /* log_e 10 */
#endif


#define REAL(x) ((x).dat[0])
#define IMAG(x) ((x).dat[1])

#define SetComp(x,r,i) {REAL(x)=r;IMAG(x)=i;}

#define COMP_SET(r,i,x) {REAL(x)=r;IMAG(x)=i;}

#define COMP_NEW(r,i) \
  rb_funcall(cComplex, rb_intern("new"), 2, rb_float_new(r), rb_float_new(i))

#define COMP2NUM(x) \
  rb_funcall(cComplex, rb_intern("new"), 2, rb_float_new(REAL(x)), rb_float_new(IMAG(x)))

#define COMP_IS_ZERO(x) (REAL(x)==0 && IMAG(x)==0)
#define COMP_EQ(x,y) (REAL(x)==REAL(y) && IMAG(x)==IMAG(y))
#define COMP_NE(x,y) (REAL(x)!=REAL(y) || IMAG(x)!=IMAG(y))
#define COMP_ISNAN(x) (isnan(REAL(x))||isnan(IMAG(x)))
#define COMP_ISINF(x) (isinf(REAL(x))||isinf(IMAG(x)))
#define COMP_ISFINITE(x) (isfinite(REAL(x))&&isfinite(IMAG(x)))

#define COMP_MINUS(x,z) {REAL(z)=-REAL(x); IMAG(z)=-IMAG(x);}

#define COMP_MUL_IMAG(x,z) \
    {real_t tmp=REAL(x); REAL(z)=-IMAG(x); IMAG(z)=tmp;}

#define COMP_ADD(x,y,z) \
    {REAL(z)=REAL(x)+REAL(y); IMAG(z)=IMAG(x)+IMAG(y);}

#define COMP_SUB(x,y,z) \
    {REAL(z)=REAL(x)-REAL(y); IMAG(z)=IMAG(x)-IMAG(y);}

#define COMP_MUL(x,y,z)					\
    { real_t im = REAL(x)*IMAG(y)+IMAG(x)*REAL(y);	\
	REAL(z) = REAL(x)*REAL(y)-IMAG(x)*IMAG(y);	\
	IMAG(z) = im; }

#define COMP_DIV(x,y,z)					\
    { real_t s,yr,yi,im;				\
	s = hypot(REAL(y),IMAG(y));			\
	yr = REAL(y)/s;					\
	yi = IMAG(y)/s;					\
	im = (IMAG(x)*yr-REAL(x)*yi)/s;			\
	REAL(z) = (REAL(x)*yr+IMAG(x)*yi)/s;		\
	IMAG(z) = im; }

#define COMP_INVERSE(x,z)				\
    {							\
	if ( fabs(REAL(x)) > fabs(IMAG(x)) ) {		\
	    IMAG(z) = IMAG(x)/REAL(x);			\
	    REAL(z) = (1+IMAG(z)*IMAG(z))*REAL(x);	\
	    IMAG(z) /= -REAL(z);			\
	    REAL(z) = 1/REAL(z);			\
	} else {					\
	    REAL(z) = REAL(x)/IMAG(x);			\
	    IMAG(z) = (1+REAL(z)*REAL(z))*IMAG(x);	\
	    REAL(z) /= IMAG(z);				\
	    IMAG(z) = -1/IMAG(z);			\
	}						\
    }

#define COMP_SQUARE(x,z)				\
    { real_t im = 2*REAL(x)*IMAG(x);			\
	REAL(z) = REAL(x)*REAL(x)-IMAG(x)*IMAG(x);	\
	IMAG(z) = im; }

#define COMP_SQRT(x,z)					\
    { real_t xr, xi, r;					\
	xr = REAL(x)/2;					\
	xi = IMAG(x)/2;					\
	r  = hypot(xr,xi);				\
	if (xr>0) {					\
	    REAL(z) = sqrt(r+xr);			\
	    IMAG(z) = xi/REAL(z);			\
	} else if ( (r-=xr)!=0 ) {			\
	    IMAG(z) = (xi>=0) ? sqrt(r):-sqrt(r);	\
	    REAL(z) = xi/IMAG(z);			\
	} else {					\
	    REAL(z) = IMAG(z) = 0;			\
	}						\
    }

#define COMP_LOG(x,z)					\
    { real_t im = atan2(IMAG(x),REAL(x));		\
	REAL(z) = log(hypot(REAL(x),IMAG(x)));		\
	IMAG(z) = im; }

#define COMP_EXP(x,z)				\
    { real_t a = exp(REAL(x));			\
	REAL(z) = a*cos(IMAG(x));		\
	IMAG(z) = a*sin(IMAG(x)); }

#define COMP_EXP2(x,z)				\
    { real_t a = exp(REAL(x)*M_LN2);		\
	REAL(z) = a*cos(IMAG(x));		\
	IMAG(z) = a*sin(IMAG(x)); }

#define COMP_EXP10(x,z)				\
    { real_t a = exp(REAL(x)*M_LN10);		\
	REAL(z) = a*cos(IMAG(x));		\
	IMAG(z) = a*sin(IMAG(x)); }

#define COMP_SIN(x,z)				\
    { real_t im = cos(REAL(x))*sinh(IMAG(x));	\
	REAL(z) = sin(REAL(x))*cosh(IMAG(x));	\
	IMAG(z) = im; }

#define COMP_SINH(x,z)				\
    { real_t im = cosh(REAL(x))*sin(IMAG(x));	\
	REAL(z) = sinh(REAL(x))*cos(IMAG(x));	\
	IMAG(z) = im; }

#define COMP_COS(x,z)				\
    { real_t im = -sin(REAL(x))*sinh(IMAG(x));	\
	REAL(z) = cos(REAL(x))*cosh(IMAG(x));	\
	IMAG(z) = im; }

#define COMP_COSH(x,z)				\
    { real_t im = sinh(REAL(x))*sin(IMAG(x));	\
	REAL(z) = cosh(REAL(x))*cos(IMAG(x));	\
	IMAG(z) = im; }

#define COMP_TAN(x,z)				\
    { real_t c, d;				\
	if (fabs(IMAG(x))<1) {			\
	    c = cos(REAL(x));			\
	    d = sinh(IMAG(x));			\
	    d = c*c + d*d;			\
	    REAL(z) = 0.5*sin(2*REAL(x))/d;	\
	    IMAG(z) = 0.5*sinh(2*IMAG(x))/d;	\
	} else {				\
	    d = exp(-IMAG(x));			\
	    c = 2*d/(1-d*d);			\
	    c = c*c;				\
	    d = cos(REAL(x));			\
	    d = 1.0 + d*d*c;			\
	    REAL(z) = 0.5*sin(2*REAL(x))*c/d;	\
	    IMAG(z) = 1/tanh(IMAG(x))/d;	\
	}					\
    }

#define COMP_TANH(x,z)				\
    { real_t c, d, s;				\
	c = cos(IMAG(x));			\
	s = sinh(REAL(x));			\
	d = c*c + s*s;				\
	if (fabs(REAL(x))<1) {			\
	    REAL(z) = s*cosh(REAL(x))/d;	\
	    IMAG(z) = 0.5*sin(2*IMAG(x))/d;	\
	} else {				\
	    c = c / s;				\
	    c = 1 + c*c;			\
	    REAL(z) = 1/(tanh(REAL(x))*c);	\
	    IMAG(z) = 0.5*sin(2*IMAG(x))/d;	\
	}					\
    }

#define COMP_ASIN(x,z)		\
    { complex_t y;		\
	COMP_SQUARE(x,y);	\
	REAL(y) = 1-REAL(y);	\
	IMAG(y) = -IMAG(y);	\
	COMP_SQRT(y,y);		\
	REAL(y) -= IMAG(x);	\
	IMAG(y) += REAL(x);	\
	COMP_LOG(y,y);		\
	REAL(z) = IMAG(y);	\
	IMAG(z) = -REAL(y);	\
    }

#define COMP_ASINH(x,z)		\
    { complex_t y;		\
	COMP_SQUARE(x,y);	\
	REAL(y) += 1;		\
	COMP_SQRT(y,y);		\
	REAL(y) += REAL(x);	\
	IMAG(y) += IMAG(x);	\
	COMP_LOG(y,z);		\
    }

#define COMP_ACOS(x,z)		\
    { complex_t y;		\
	COMP_SQUARE(x,y);	\
	REAL(y) = 1-REAL(y);	\
	IMAG(y) = -IMAG(y);	\
	COMP_SQRT(y,y);		\
	REAL(z) = REAL(x)-IMAG(y);\
	IMAG(z) = IMAG(x)+REAL(y);\
	COMP_LOG(z,y);		\
	REAL(z) = IMAG(y);	\
	IMAG(z) = -REAL(y);	\
    }

#define COMP_ACOSH(x,z)		\
    { complex_t y;		\
	COMP_SQUARE(x,y);	\
	REAL(y) -= 1;		\
	COMP_SQRT(y,y);		\
	REAL(y) += REAL(x);	\
	IMAG(y) += IMAG(x);	\
	COMP_LOG(y,z);		\
    }

#define COMP_ATAN(x,z)		\
    { complex_t y;		\
	REAL(y) = -REAL(x);	\
	IMAG(y) = 1-IMAG(x);	\
	REAL(z) = REAL(x);	\
	IMAG(z) = 1+IMAG(x);	\
	COMP_DIV(z,y,y);	\
	COMP_LOG(y,y);		\
	REAL(z) = -IMAG(y)/2;	\
	IMAG(z) = REAL(y)/2;	\
    }

#define COMP_ATANH(x,z)		\
    { complex_t y;		\
	REAL(y) = 1-REAL(x);	\
	IMAG(y) = -IMAG(x);	\
	REAL(z) = 1+REAL(x);	\
	IMAG(z) = IMAG(x);	\
	COMP_DIV(z,y,y);	\
	COMP_LOG(y,y);		\
	REAL(z) = REAL(y)/2;	\
	IMAG(z) = IMAG(y)/2;	\
    }

#define COMP_POW(x,y,z)						\
    {	if (COMP_IS_ZERO(y)) {					\
	    COMP_SET(1,0,z);					\
	} else if (COMP_IS_ZERO(x) && REAL(y)>0 && IMAG(y)==0) {\
	    COMP_SET(0,0,z);					\
	} else {						\
	    COMP_LOG(x,z);					\
	    COMP_MUL(y,z,z);					\
	    COMP_EXP(z,z);					\
	}							\
    }

#define COMP_POW_INT(type)			\
static type pow_##type##_int(type x, int p)	\
{						\
    type z={1,0};				\
    if (p<0) {					\
	x = pow_##type##_int(x,-p);		\
	COMP_INVERSE(x,x);			\
	return x;				\
    }						\
    if (p==2) {COMP_SQUARE(x,z); return z;}	\
    if (p&1) { z = x; }				\
    p>>=1;					\
    while (p) {					\
	COMP_SQUARE(x,x);			\
	if (p&1) COMP_MUL(z,x,z);		\
	p>>=1;					\
    }						\
    return z;					\
}

