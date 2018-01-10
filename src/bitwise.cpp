#include <cassert>
#include <cstdint>
#include <cmath>
#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

extern "C"
SEXP bitwise_integer_to_double(SEXP ival)
{
    double *ival_cpp = REAL(ival);
    R_xlen_t n = Rf_xlength(ival);
    SEXP dval = Rf_protect(Rf_allocVector(REALSXP, n));
    double *dval_cpp = REAL(dval);
    for (R_xlen_t i=0; i<n; i++) {
        assert(!(R_IsNA(ival_cpp[i]) || R_IsNaN(ival_cpp[i])));
        int64_t tmp = static_cast<int64_t>(ival_cpp[i]);
        double intpart;
        if (std::modf(ival_cpp[i], &intpart)!=0.0) {
            Rf_error("Although the type of inputs could be"
                     " non-integer, the values must be integers");
        }
        if (static_cast<double>(tmp)!=ival_cpp[i]) {
            Rf_error("Input integer value is not precisely representable.");
        } 
        dval_cpp[i] = reinterpret_cast<double&>(tmp);
    }
    Rf_unprotect(1);
    return dval;
}

extern "C"
SEXP bitwise_double_to_integer(SEXP dval)
{
    double *dval_cpp = REAL(dval);
    R_xlen_t n = Rf_xlength(dval);
    SEXP ival = Rf_protect(Rf_allocVector(REALSXP, n));
    double *ival_cpp = REAL(ival);
    for (R_xlen_t i=0; i<n; i++) {
        int64_t *tmp = reinterpret_cast<int64_t*>(dval_cpp+i);
        //if (*tmp > 9007199254740991) {
        if (*tmp != static_cast<int64_t>(static_cast<double>(*tmp))) {
            Rf_error("result integer value is not precisely representable.");
        }
        ival_cpp[i] = static_cast<double>(*tmp);
    }
    Rf_unprotect(1);
    return ival;
}
