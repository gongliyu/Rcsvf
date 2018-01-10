#' Construct a chunk from offset values of beginning and end
#'
#'
chunk <- function(begin, end, bit.encoded=FALSE)
{
  if (!isTRUE(bit.encoded)) {
    begin = bitwise.integer2double(begin)
    end = bitwise.integer2double(end)
  }
  res <- list(begin=begin, end=end)
  class(res) <- "chunk"
  if (!is.valid.chunk(res)) {
    stop(paste("Input \"begin\" and \"end\" does not form a valid",
               "chunk. Please make sure end > begin."))
  }
  return(res)
}
