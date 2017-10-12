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
    initialize = function(filename)
    {
      private$handle <- .Call("rcsvf_reader_create")
      self$open(filename)
    },

    open = function(filename, sep=",", eol="\n",
                    quote.rule="doubled", fill=TRUE,
                    strip.white=TRUE, skip.blank.lines=TRUE,
                    quote="\"", verbose=TRUE,
                    begin.offset=NULL, end.offset=NULL)
    {
      .Call("rcsvf_reader_open", private$handle,
            filename, sep, eol, quote.rule, fill, strip.white,
            skip.blank.lines, quote, verbose,
            begin.offset, end.offset)
    },
    
    finalize = function()
    {
      if (!is.null(private$handle))
        .Call("rcsvf_reader_destroy", private$handle)
    }),

  private = list(
    handle = NULL)
)


    
