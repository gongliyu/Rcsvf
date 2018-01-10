is.valid.chunk <- function(chunk)
{
  if (!is.list(chunk)) {
    return("chunk is not a list")
  }

  if (!"begin" %in% names(chunk)) {
    return("chunk does not contain field \"begin\"")
  }

  if (!"end" %in% names(chunk)) {
    return("chunk does not contain field \"end\"")
  }

  
}
