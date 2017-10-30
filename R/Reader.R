#' csv file reader which can read the file sequentially
#'
#' @docType class
#' @export
#' @import R6
#' @keywords data
#'
#' @useDynLib csvf
#'

Reader <- R6::R6Class(
  "Reader",
  public = list(
    initialize = function(...)
    {
      private$handle <- .Call("rcsvf_reader_create")
      self$open(...)
    },

    open = function(filename, sep=",", eol="\n",
                    quote.rule="doubled", fill=TRUE,
                    strip.white=TRUE, skip.blank.lines=TRUE,
                    quote="\"", verbose=FALSE,
                    begin.offset=NULL, end.offset=NULL,
                    header="auto")
    {
      .Call("rcsvf_reader_open", private$handle,
            filename, sep, eol, quote.rule, fill, strip.white,
            skip.blank.lines, quote, verbose,
            begin.offset, end.offset, header)
    },
    
    finalize = function()
    {
      if (!is.null(private$handle))
        .Call("rcsvf_reader_destroy", private$handle)
    },

    properties = function()
    {
      .Call("rcsvf_reader_properties", private$handle)
    },

    read = function(n=1)
    {
      .Call("rcsvf_reader_read", private$handle, n)
    }
    ),

  private = list(
    handle = NULL)
)


    
