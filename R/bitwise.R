#' Bitwise value conversion
#'
#' Reinterpret a value of one type as a bitwise identical value of
#' another type. After reinterpretion, the values are different, but
#' the underlying bits representation with respect to the two types
#' are the same.
#'
#' These functions mimic the \code{reinterpret_cast<A&>(val)}
#' functionality of C++. Actually, they are implemented using C++
#' \code{reinterpret_cast}.
#'
#' @param ival integer values, could be integer, double, logical type,
#'   but must be integer values.
#' 
#' @param dval double values of double type.
#' 
bitwise.integer2double <- function(ival)
{
  ## check input argument
  if (!is.integer(ival) && !is.double(ival) && !is.logical(ival)) {
    stop("Input should be type double, integer or logical.")
  }
  
  ## Since integer in R are at most 32 bit, so we can simply store
  ## integer value in a double variable without lose any precision.
  ival <- as.double(ival)

  if (any(is.na(ival))) {
    stop("Input contains NA or NaN which are not integer values.")
  }

  dval <- .Call("bitwise_integer_to_double", ival)
  return(dval)
}

bitwise.double2integer <- function(dval)
{
  ## if (!is.null(dval) && all(is.na(dval))) {
  ##   return(rep(NA,length(dval)))
  ## }
  
  ## check input argument
  if (!is.double(dval)) {
    stop("Input should be double type.")
  }

  .Call("bitwise_double_to_integer", dval)
}

