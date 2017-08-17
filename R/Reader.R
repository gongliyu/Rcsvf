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
      private$handle <- csvf_reader_create(filename);
    },

    finalize = function()
    {
      csvf_reader_destroy(private$handle);
    }),

  private = list(
    handle = NULL)
)


    
