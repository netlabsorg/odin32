/* math.h (emx+gcc) */

#ifndef _EMXMATH_H
#define _EMXMATH_H

#ifndef CRTWRAP
#define CRTWRAP(a)   emx_##a
#endif

#if defined (__cplusplus)
extern "C" {
#endif

#define HUGE_VAL        1e9999

#define acos CRTWRAP(acos)
#define asin CRTWRAP(asin)
#define atan CRTWRAP(atan)
#define atan2 CRTWRAP(atan2)
#define cbrt CRTWRAP(cbrt)
#define ceil CRTWRAP(ceil)
#define cos CRTWRAP(cos)
#define cosh CRTWRAP(cosh)
#define exp CRTWRAP(exp)
#define fabs CRTWRAP(fabs)
#define floor CRTWRAP(floor)
#define fmod CRTWRAP(fmod)
#define frexp CRTWRAP(frexp)
#define hypot CRTWRAP(hypot)
#define ldexp CRTWRAP(ldexp)
#define log CRTWRAP(log)
#define log10 CRTWRAP(log10)
#define modf CRTWRAP(modf)
#define pow CRTWRAP(pow)
#define rint CRTWRAP(rint)
#define sin CRTWRAP(sin)
#define sinh CRTWRAP(sinh)
#define sqrt CRTWRAP(sqrt)
#define tan CRTWRAP(tan)
#define tanh CRTWRAP(tanh)
#define trunc CRTWRAP(trunc)

double acos (double);
double asin (double);
double atan (double);
double atan2 (double, double);
double cbrt (double);
double ceil (double);
double cos (double);
double cosh (double);
double exp (double);
double fabs (double);
double floor (double);
double fmod (double, double);
double frexp (double, int *);
double hypot (double, double);
double ldexp (double, int);
double log (double);
double log10 (double);
double modf (double, double *);
double pow (double, double);
double rint (double);
double sin (double);
double sinh (double);
double sqrt (double);
double tan (double);
double tanh (double);
double trunc (double);

#if !defined (__NO_C9X)

#define HUGE_VALF       1e9999F
#define HUGE_VALL       1e9999L
#define INFINITY        1e9999          /* TODO: float_t */
#define NAN             (0.0/0.0)       /* TODO: Exceptions, float_t */

#define DECIMAL_DIG     21

#define FP_ZERO         0
#define FP_SUBNORMAL    1
#define FP_NORMAL       2
#define FP_INFINITE     3
#define FP_NAN          4

#define fpclassify(x) \
    ((sizeof (x) == sizeof (float))  ? emx___fpclassifyf (x) \
   : (sizeof (x) == sizeof (double)) ? emx___fpclassify (x) \
   :                                   emx___fpclassifyl (x))

#define signbit(x) \
    ((sizeof (x) == sizeof (float))  ? emx___signbitf (x) \
   : (sizeof (x) == sizeof (double)) ? emx___signbit (x) \
   :                                   emx___signbitl (x))

#define isfinite(x) \
    ((sizeof (x) == sizeof (float))  ? emx___isfinitef (x) \
   : (sizeof (x) == sizeof (double)) ? emx___isfinite (x) \
   :                                   emx___isfinitel (x))

#define isnormal(x) \
    ((sizeof (x) == sizeof (float))  ? emx___isnormalf (x) \
   : (sizeof (x) == sizeof (double)) ? emx___isnormal (x) \
   :                                   emx___isnormall (x))

#define isnan(x) \
    ((sizeof (x) == sizeof (float))  ? emx___isnanf (x) \
   : (sizeof (x) == sizeof (double)) ? emx___isnan (x) \
   :                                   emx___isnanl (x))

#define copysignf CRTWRAP(copysignf)
#define copysign CRTWRAP(copysign)
#define copysignl CRTWRAP(copysignl)

float copysignf (float, float);
double copysign (double, double);
long double copysignl (long double, long double);

#define nextafterf CRTWRAP(nextafterf)
#define nextafter CRTWRAP(nextafter)
#define nextafterl CRTWRAP(nextafterl)

float nextafterf (float, float);
double nextafter (double, double);
long double nextafterl (long double, long double);

#define acosl CRTWRAP(acosl)
#define asinl CRTWRAP(asinl)
#define atanl CRTWRAP(atanl)
#define atan2l CRTWRAP(atan2l)
#define cbrtl CRTWRAP(cbrtl)
#define ceill CRTWRAP(ceill)
#define cosl CRTWRAP(cosl)
#define coshl CRTWRAP(coshl)
#define expl CRTWRAP(expl)
#define fabsl CRTWRAP(fabsl)
#define floorl CRTWRAP(floorl)
#define fmodl CRTWRAP(fmodl)
#define frexpl CRTWRAP(frexpl)
#define hypotl CRTWRAP(hypotl)
#define ldexpl CRTWRAP(ldexpl)
#define logl CRTWRAP(logl)
#define log10l CRTWRAP(log10l)
#define modfl CRTWRAP(modfl)
#define powl CRTWRAP(powl)
#define rintl CRTWRAP(rintl)
#define sinl CRTWRAP(sinl)
#define sinhl CRTWRAP(sinhl)
#define sqrtl CRTWRAP(sqrtl)
#define tanl CRTWRAP(tanl)
#define tanhl CRTWRAP(tanhl)
#define truncl CRTWRAP(truncl)

long double acosl (long double);
long double asinl (long double);
long double atanl (long double);
long double atan2l (long double, long double);
long double cbrtl (long double);
long double ceill (long double);
long double cosl (long double);
long double coshl (long double);
long double expl (long double);
long double fabsl (long double);
long double floorl (long double);
long double fmodl (long double, long double);
long double frexpl (long double, int *);
long double hypotl (long double, long double);
long double ldexpl (long double, int);
long double logl (long double);
long double log10l (long double);
long double modfl (long double, long double *);
long double powl (long double, long double);
long double rintl (long double);
long double sinl (long double);
long double sinhl (long double);
long double sqrtl (long double);
long double tanl (long double);
long double tanhl (long double);
long double truncl (long double);

/* Internal funcs, no change */

int emx___fpclassify (double);
int emx___fpclassifyf (float);
int emx___fpclassifyl (long double);

int emx___signbit (double);
int emx___signbitf (float);
int emx___signbitl (long double);

int emx___isfinite (double);
int emx___isfinitef (float);
int emx___isfinitel (long double);

int emx___isnormal (double);
int emx___isnormalf (float);
int emx___isnormall (long double);

int emx___isnan (double);
int emx___isnanf (float);
int emx___isnanl (long double);

#endif

#if !defined (__STRICT_ANSI__)

#define HUGE    HUGE_VAL

#define atof CRTWRAP(atof)
double atof (__const__ char *);

#if !defined (_ABS)                                    /* see also stdlib.h */
#define _ABS
#if !defined (__GNUC__) || __GNUC__ >= 2

#define abs CRTWRAP(abs)
#define labs CRTWRAP(labs)

extern int abs (int);
extern long labs (long);
#else
extern __inline__ int abs (int _n) { return (_n < 0 ? -_n : _n); }
extern __inline__ long labs (long _n) { return (_n < 0 ? -_n : _n); }
#endif
#endif

#endif


#if !defined (__STRICT_ANSI__) || defined (_WITH_UNDERSCORE)

#define _HUGE_VALF      1e9999F
#define _HUGE_VALL      1e9999L
#define _INFINITY       1e9999          /* TODO: float_t */
#define _NAN            (0.0/0.0)       /* TODO: Exceptions, float_t */

#define _LHUGE_VAL      _HUGE_VALL
#define _LHUGE          _HUGE_VALL

#define _copysignf CRTWRAP(_copysignf)
#define _copysign CRTWRAP(_copysign)
#define _copysignl CRTWRAP(_copysignl)

float _copysignf (float, float);
double _copysign (double, double);
long double _copysignl (long double, long double);

#define _nextafterf CRTWRAP(_nextafterf)
#define _nextafter CRTWRAP(_nextafter)
#define _nextafterl CRTWRAP(_nextafterl)

float _nextafterf (float, float);
double _nextafter (double, double);
long double _nextafterl (long double, long double);

#define _acosl CRTWRAP(_acosl)
#define _asinl CRTWRAP(_asinl)
#define _atanl CRTWRAP(_atanl)
#define _atan2l CRTWRAP(_atan2l)
#define _atofl CRTWRAP(_atofl)
#define _cbrtl CRTWRAP(_cbrtl)
#define _ceill CRTWRAP(_ceill)
#define _cosl CRTWRAP(_cosl)
#define _coshl CRTWRAP(_coshl)
#define _hypotl CRTWRAP(_hypotl)
#define _expl CRTWRAP(_expl)
#define _fabsl CRTWRAP(_fabsl)
#define _floorl CRTWRAP(_floorl)
#define _fmodl CRTWRAP(_fmodl)
#define _frexpl CRTWRAP(_frexpl)
#define _ldexpl CRTWRAP(_ldexpl)
#define _logl CRTWRAP(_logl)
#define _log10l CRTWRAP(_log10l)
#define _modfl CRTWRAP(_modfl)
#define _powl CRTWRAP(_powl)
#define _rintl CRTWRAP(_rintl)
#define _sinl CRTWRAP(_sinl)
#define _sinhl CRTWRAP(_sinhl)
#define _sqrtl CRTWRAP(_sqrtl)
#define _tanl CRTWRAP(_tanl)
#define _tanhl CRTWRAP(_tanhl)
#define _truncl CRTWRAP(_truncl)

long double _acosl (long double);
long double _asinl (long double);
long double _atanl (long double);
long double _atan2l (long double, long double);
long double _atofl (__const__ char *);
long double _cbrtl (long double);
long double _ceill (long double);
long double _cosl (long double);
long double _coshl (long double);
long double _hypotl (long double, long double);
long double _expl (long double);
long double _fabsl (long double);
long double _floorl (long double);
long double _fmodl (long double, long double);
long double _frexpl (long double, int *);
long double _ldexpl (long double, int);
long double _logl (long double);
long double _log10l (long double);
long double _modfl (long double, long double *);
long double _powl (long double, long double);
long double _rintl (long double);
long double _sinl (long double);
long double _sinhl (long double);
long double _sqrtl (long double);
long double _tanl (long double);
long double _tanhl (long double);
long double _truncl (long double);

/* Emx Extended Math - No prefixes */
double y0(double);
double y1(double);
double yn(int,double);

#endif


#if defined (__cplusplus)
}
#endif

#endif /* not _MATH_H */
