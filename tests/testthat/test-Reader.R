context("Reader")

test_that("constructor", {
  reader <- Reader$new("data/simple.csv")
  expect_equal(2 * 2, 4)
})

test_that("properties", {
  reader <- Reader$new("data/simple.csv")
  expect_identical(
    reader$properties(),
    list(sep=",", eol="\n", quote.rule="doubled", fill=TRUE,
         strip.white=TRUE, skip.blank.lines=TRUE,
         quote="\"", verbose=FALSE, nfields=as.integer(5),
         field.types=rep("integer",5)))
})

test_that("detect field types", {
  reader <- Reader$new("data/mixnumchar.csv")
  print(reader$properties())
  expect_identical(
    reader$properties()$field.types,
    c("character","double","integer"))
})


