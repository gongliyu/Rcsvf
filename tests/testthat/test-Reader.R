context("Reader")

test_that("constructor", {
  Reader$new("data/simple.csv")
  expect_equal(2 * 2, 4)
})
