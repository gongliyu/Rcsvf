#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

extern "C"
SEXP is_valid_chunk(SEXP begin, SEXP end)
{
    double begin_cpp = Rf_asReal(begin);
    double end_cpp = Rf_asReal(end);
    return begin_cpp >=0 && end_cpp >= 0 && end_cpp >= begin_cpp;
}
