context("bitwise")

test_that("bitwise", {
  expect_identical(bitwise.double2integer(bitwise.integer2double(1)), 1)
  expect_identical(bitwise.double2integer(bitwise.integer2double(-1)), -1)
  expect_identical(bitwise.double2integer(bitwise.integer2double(10)), 10)
  expect_identical(bitwise.double2integer(bitwise.integer2double(2^31-1)), 2^31-1)
  expect_identical(bitwise.double2integer(bitwise.integer2double(-2^31)), -2^31)
})
  
